// SPDX-License-Identifier: GPL-2.0
/*
 * USB-ACPI glue code
 *
 * Copyright 2012 Red Hat <mjg@redhat.com>
 */
#include <freax/module.h>
#include <freax/usb.h>
#include <freax/device.h>
#include <freax/errno.h>
#include <freax/kernel.h>
#include <freax/acpi.h>
#include <freax/pci.h>
#include <freax/usb/hcd.h>

#include "hub.h"

/**
 * usb_acpi_power_manageable - check whether usb port has
 * acpi power resource.
 * @hdev: USB device belonging to the usb hub
 * @index: port index based zero
 *
 * Return true if the port has acpi power resource and false if no.
 */
bool usb_acpi_power_manageable(struct usb_device *hdev, int index)
{
	acpi_handle port_handle;
	int port1 = index + 1;

	port_handle = usb_get_hub_port_acpi_handle(hdev,
		port1);
	if (port_handle)
		return acpi_bus_power_manageable(port_handle);
	else
		return false;
}
EXPORT_SYMBOL_GPL(usb_acpi_power_manageable);

#define UUID_USB_CONTROLLER_DSM "ce2ee385-00e6-48cb-9f05-2edb927c4899"
#define USB_DSM_DISABLE_U1_U2_FOR_PORT	5

/**
 * usb_acpi_port_lpm_incapable - check if lpm should be disabled for a port.
 * @hdev: USB device belonging to the usb hub
 * @index: zero based port index
 *
 * Some USB3 ports may not support USB3 link power management U1/U2 states
 * due to different retimer setup. ACPI provides _DSM method which returns 0x01
 * if U1 and U2 states should be disabled. Evaluate _DSM with:
 * Arg0: UUID = ce2ee385-00e6-48cb-9f05-2edb927c4899
 * Arg1: Revision ID = 0
 * Arg2: Function Index = 5
 * Arg3: (empty)
 *
 * Return 1 if USB3 port is LPM incapable, negative on error, otherwise 0
 */

int usb_acpi_port_lpm_incapable(struct usb_device *hdev, int index)
{
	union acpi_object *obj;
	acpi_handle port_handle;
	int port1 = index + 1;
	guid_t guid;
	int ret;

	ret = guid_parse(UUID_USB_CONTROLLER_DSM, &guid);
	if (ret)
		return ret;

	port_handle = usb_get_hub_port_acpi_handle(hdev, port1);
	if (!port_handle) {
		dev_dbg(&hdev->dev, "port-%d no acpi handle\n", port1);
		return -ENODEV;
	}

	if (!acpi_check_dsm(port_handle, &guid, 0,
			    BIT(USB_DSM_DISABLE_U1_U2_FOR_PORT))) {
		dev_dbg(&hdev->dev, "port-%d no _DSM function %d\n",
			port1, USB_DSM_DISABLE_U1_U2_FOR_PORT);
		return -ENODEV;
	}

	obj = acpi_evaluate_dsm_typed(port_handle, &guid, 0,
				      USB_DSM_DISABLE_U1_U2_FOR_PORT, NULL,
				      ACPI_TYPE_INTEGER);
	if (!obj) {
		dev_dbg(&hdev->dev, "evaluate port-%d _DSM failed\n", port1);
		return -EINVAL;
	}

	if (obj->integer.value == 0x01)
		ret = 1;

	ACPI_FREE(obj);

	return ret;
}
EXPORT_SYMBOL_GPL(usb_acpi_port_lpm_incapable);

/**
 * usb_acpi_set_power_state - control usb port's power via acpi power
 * resource
 * @hdev: USB device belonging to the usb hub
 * @index: port index based zero
 * @enable: power state expected to be set
 *
 * Notice to use usb_acpi_power_manageable() to check whether the usb port
 * has acpi power resource before invoking this function.
 *
 * Returns 0 on success, else negative errno.
 */
int usb_acpi_set_power_state(struct usb_device *hdev, int index, bool enable)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(hdev);
	struct usb_port *port_dev;
	acpi_handle port_handle;
	unsigned char state;
	int port1 = index + 1;
	int error = -EINVAL;

	if (!hub)
		return -ENODEV;
	port_dev = hub->ports[port1 - 1];

	port_handle = (acpi_handle) usb_get_hub_port_acpi_handle(hdev, port1);
	if (!port_handle)
		return error;

	if (enable)
		state = ACPI_STATE_D0;
	else
		state = ACPI_STATE_D3_COLD;

	error = acpi_bus_set_power(port_handle, state);
	if (!error)
		dev_dbg(&port_dev->dev, "acpi: power was set to %d\n", enable);
	else
		dev_dbg(&port_dev->dev, "acpi: power failed to be set\n");

	return error;
}
EXPORT_SYMBOL_GPL(usb_acpi_set_power_state);

static enum usb_port_connect_type usb_acpi_get_connect_type(acpi_handle handle,
		struct acpi_pld_info *pld)
{
	enum usb_port_connect_type connect_type = USB_PORT_CONNECT_TYPE_UNKNOWN;
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *upc = NULL;
	acpi_status status;

	/*
	 * According to 9.14 in ACPI Spec 6.2. _PLD indicates whether usb port
	 * is user visible and _UPC indicates whether it is connectable. If
	 * the port was visible and connectable, it could be freely connected
	 * and disconnected with USB devices. If no visible and connectable,
	 * a usb device is directly hard-wired to the port. If no visible and
	 * no connectable, the port would be not used.
	 */
	status = acpi_evaluate_object(handle, "_UPC", NULL, &buffer);
	if (ACPI_FAILURE(status))
		goto out;

	upc = buffer.pointer;
	if (!upc || (upc->type != ACPI_TYPE_PACKAGE) || upc->package.count != 4)
		goto out;

	if (upc->package.elements[0].integer.value)
		if (pld->user_visible)
			connect_type = USB_PORT_CONNECT_TYPE_HOT_PLUG;
		else
			connect_type = USB_PORT_CONNECT_TYPE_HARD_WIRED;
	else if (!pld->user_visible)
		connect_type = USB_PORT_NOT_USED;
out:
	kfree(upc);
	return connect_type;
}


/*
 * Private to usb-acpi, all the core needs to know is that
 * port_dev->location is non-zero when it has been set by the firmware.
 */
#define USB_ACPI_LOCATION_VALID (1 << 31)

static struct acpi_device *
usb_acpi_get_companion_for_port(struct usb_port *port_dev)
{
	struct usb_device *udev;
	struct acpi_device *adev;
	acpi_handle *parent_handle;
	int port1;

	/* Get the struct usb_device point of port's hub */
	udev = to_usb_device(port_dev->dev.parent->parent);

	/*
	 * The root hub ports' parent is the root hub. The non-root-hub
	 * ports' parent is the parent hub port which the hub is
	 * connected to.
	 */
	if (!udev->parent) {
		adev = ACPI_COMPANION(&udev->dev);
		port1 = usb_hcd_find_raw_port_number(bus_to_hcd(udev->bus),
						     port_dev->portnum);
	} else {
		parent_handle = usb_get_hub_port_acpi_handle(udev->parent,
							     udev->portnum);
		if (!parent_handle)
			return NULL;

		adev = acpi_fetch_acpi_dev(parent_handle);
		port1 = port_dev->portnum;
	}

	return acpi_find_child_by_adr(adev, port1);
}

static struct acpi_device *
usb_acpi_find_companion_for_port(struct usb_port *port_dev)
{
	struct acpi_device *adev;
	struct acpi_pld_info *pld;
	acpi_handle *handle;
	acpi_status status;

	adev = usb_acpi_get_companion_for_port(port_dev);
	if (!adev)
		return NULL;

	handle = adev->handle;
	status = acpi_get_physical_device_location(handle, &pld);
	if (ACPI_SUCCESS(status) && pld) {
		port_dev->location = USB_ACPI_LOCATION_VALID
			| pld->group_token << 8 | pld->group_position;
		port_dev->connect_type = usb_acpi_get_connect_type(handle, pld);
		ACPI_FREE(pld);
	}

	return adev;
}

static struct acpi_device *
usb_acpi_find_companion_for_device(struct usb_device *udev)
{
	struct acpi_device *adev;
	struct usb_port *port_dev;
	struct usb_hub *hub;

	if (!udev->parent) {
		/*
		 * root hub is only child (_ADR=0) under its parent, the HC.
		 * sysdev pointer is the HC as seen from firmware.
		 */
		adev = ACPI_COMPANION(udev->bus->sysdev);
		return acpi_find_child_device(adev, 0, false);
	}

	hub = usb_hub_to_struct_hub(udev->parent);
	if (!hub)
		return NULL;

	/*
	 * This is an embedded USB device connected to a port and such
	 * devices share port's ACPI companion.
	 */
	port_dev = hub->ports[udev->portnum - 1];
	return usb_acpi_get_companion_for_port(port_dev);
}

static struct acpi_device *usb_acpi_find_companion(struct device *dev)
{
	/*
	 * The USB hierarchy like following:
	 *
	 * Device (EHC1)
	 *	Device (HUBN)
	 *		Device (PR01)
	 *			Device (PR11)
	 *			Device (PR12)
	 *				Device (FN12)
	 *				Device (FN13)
	 *			Device (PR13)
	 *			...
	 * where HUBN is root hub, and PRNN are USB ports and devices
	 * connected to them, and FNNN are individualk functions for
	 * connected composite USB devices. PRNN and FNNN may contain
	 * _CRS and other methods describing sideband resources for
	 * the connected device.
	 *
	 * On the kernel side both root hub and embedded USB devices are
	 * represented as instances of usb_device structure, and ports
	 * are represented as usb_port structures, so the whole process
	 * is split into 2 parts: finding companions for devices and
	 * finding companions for ports.
	 *
	 * Note that we do not handle individual functions of composite
	 * devices yet, for that we would need to assign companions to
	 * devices corresponding to USB interfaces.
	 */
	if (is_usb_device(dev))
		return usb_acpi_find_companion_for_device(to_usb_device(dev));
	else if (is_usb_port(dev))
		return usb_acpi_find_companion_for_port(to_usb_port(dev));

	return NULL;
}

static bool usb_acpi_bus_match(struct device *dev)
{
	return is_usb_device(dev) || is_usb_port(dev);
}

static struct acpi_bus_type usb_acpi_bus = {
	.name = "USB",
	.match = usb_acpi_bus_match,
	.find_companion = usb_acpi_find_companion,
};

int usb_acpi_register(void)
{
	return register_acpi_bus_type(&usb_acpi_bus);
}

void usb_acpi_unregister(void)
{
	unregister_acpi_bus_type(&usb_acpi_bus);
}
