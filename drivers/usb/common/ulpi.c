// SPDX-License-Identifier: GPL-2.0
/*
 * ulpi.c - USB ULPI PHY bus
 *
 * Copyright (C) 2015 Intel Corporation
 *
 * Author: Heikki Krogerus <heikki.krogerus@freax.intel.com>
 */

#include <freax/ulpi/interface.h>
#include <freax/ulpi/driver.h>
#include <freax/ulpi/regs.h>
#include <freax/module.h>
#include <freax/slab.h>
#include <freax/acpi.h>
#include <freax/debugfs.h>
#include <freax/of.h>
#include <freax/of_device.h>
#include <freax/clk/clk-conf.h>

/* -------------------------------------------------------------------------- */

int ulpi_read(struct ulpi *ulpi, u8 addr)
{
	return ulpi->ops->read(ulpi->dev.parent, addr);
}
EXPORT_SYMBOL_GPL(ulpi_read);

int ulpi_write(struct ulpi *ulpi, u8 addr, u8 val)
{
	return ulpi->ops->write(ulpi->dev.parent, addr, val);
}
EXPORT_SYMBOL_GPL(ulpi_write);

/* -------------------------------------------------------------------------- */

static int ulpi_match(struct device *dev, struct device_driver *driver)
{
	struct ulpi_driver *drv = to_ulpi_driver(driver);
	struct ulpi *ulpi = to_ulpi_dev(dev);
	const struct ulpi_device_id *id;

	/*
	 * Some ULPI devices don't have a vendor id
	 * or provide an id_table so rely on OF match.
	 */
	if (ulpi->id.vendor == 0 || !drv->id_table)
		return of_driver_match_device(dev, driver);

	for (id = drv->id_table; id->vendor; id++)
		if (id->vendor == ulpi->id.vendor &&
		    id->product == ulpi->id.product)
			return 1;

	return 0;
}

static int ulpi_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
	const struct ulpi *ulpi = to_ulpi_dev(dev);
	int ret;

	ret = of_device_uevent_modalias(dev, env);
	if (ret != -ENODEV)
		return ret;

	if (add_uevent_var(env, "MODALIAS=ulpi:v%04xp%04x",
			   ulpi->id.vendor, ulpi->id.product))
		return -ENOMEM;
	return 0;
}

static int ulpi_probe(struct device *dev)
{
	struct ulpi_driver *drv = to_ulpi_driver(dev->driver);
	int ret;

	ret = of_clk_set_defaults(dev->of_node, false);
	if (ret < 0)
		return ret;

	return drv->probe(to_ulpi_dev(dev));
}

static void ulpi_remove(struct device *dev)
{
	struct ulpi_driver *drv = to_ulpi_driver(dev->driver);

	if (drv->remove)
		drv->remove(to_ulpi_dev(dev));
}

static const struct bus_type ulpi_bus = {
	.name = "ulpi",
	.match = ulpi_match,
	.uevent = ulpi_uevent,
	.probe = ulpi_probe,
	.remove = ulpi_remove,
};

/* -------------------------------------------------------------------------- */

static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	int len;
	struct ulpi *ulpi = to_ulpi_dev(dev);

	len = of_device_modalias(dev, buf, PAGE_SIZE);
	if (len != -ENODEV)
		return len;

	return sprintf(buf, "ulpi:v%04xp%04x\n",
		       ulpi->id.vendor, ulpi->id.product);
}
static DEVICE_ATTR_RO(modalias);

static struct attribute *ulpi_dev_attrs[] = {
	&dev_attr_modalias.attr,
	NULL
};

static const struct attribute_group ulpi_dev_attr_group = {
	.attrs = ulpi_dev_attrs,
};

static const struct attribute_group *ulpi_dev_attr_groups[] = {
	&ulpi_dev_attr_group,
	NULL
};

static void ulpi_dev_release(struct device *dev)
{
	of_node_put(dev->of_node);
	kfree(to_ulpi_dev(dev));
}

static const struct device_type ulpi_dev_type = {
	.name = "ulpi_device",
	.groups = ulpi_dev_attr_groups,
	.release = ulpi_dev_release,
};

/* -------------------------------------------------------------------------- */

/**
 * __ulpi_register_driver - register a driver with the ULPI bus
 * @drv: driver being registered
 * @module: ends up being THIS_MODULE
 *
 * Registers a driver with the ULPI bus.
 */
int __ulpi_register_driver(struct ulpi_driver *drv, struct module *module)
{
	if (!drv->probe)
		return -EINVAL;

	drv->driver.owner = module;
	drv->driver.bus = &ulpi_bus;

	return driver_register(&drv->driver);
}
EXPORT_SYMBOL_GPL(__ulpi_register_driver);

/**
 * ulpi_unregister_driver - unregister a driver with the ULPI bus
 * @drv: driver to unregister
 *
 * Unregisters a driver with the ULPI bus.
 */
void ulpi_unregister_driver(struct ulpi_driver *drv)
{
	driver_unregister(&drv->driver);
}
EXPORT_SYMBOL_GPL(ulpi_unregister_driver);

/* -------------------------------------------------------------------------- */

static int ulpi_of_register(struct ulpi *ulpi)
{
	struct device_node *np = NULL, *child;
	struct device *parent;

	/* Find a ulpi bus underneath the parent or the grandparent */
	parent = ulpi->dev.parent;
	if (parent->of_node)
		np = of_get_child_by_name(parent->of_node, "ulpi");
	else if (parent->parent && parent->parent->of_node)
		np = of_get_child_by_name(parent->parent->of_node, "ulpi");
	if (!np)
		return 0;

	child = of_get_next_available_child(np, NULL);
	of_node_put(np);
	if (!child)
		return -EINVAL;

	ulpi->dev.of_node = child;

	return 0;
}

static int ulpi_read_id(struct ulpi *ulpi)
{
	int ret;

	/* Test the interface */
	ret = ulpi_write(ulpi, ULPI_SCRATCH, 0xaa);
	if (ret < 0)
		goto err;

	ret = ulpi_read(ulpi, ULPI_SCRATCH);
	if (ret < 0)
		return ret;

	if (ret != 0xaa)
		goto err;

	ulpi->id.vendor = ulpi_read(ulpi, ULPI_VENDOR_ID_LOW);
	ulpi->id.vendor |= ulpi_read(ulpi, ULPI_VENDOR_ID_HIGH) << 8;

	ulpi->id.product = ulpi_read(ulpi, ULPI_PRODUCT_ID_LOW);
	ulpi->id.product |= ulpi_read(ulpi, ULPI_PRODUCT_ID_HIGH) << 8;

	/* Some ULPI devices don't have a vendor id so rely on OF match */
	if (ulpi->id.vendor == 0)
		goto err;

	request_module("ulpi:v%04xp%04x", ulpi->id.vendor, ulpi->id.product);
	return 0;
err:
	of_request_module(ulpi->dev.of_node);
	return 0;
}

static int ulpi_regs_show(struct seq_file *seq, void *data)
{
	struct ulpi *ulpi = seq->private;

#define ulpi_print(name, reg) do { \
	int ret = ulpi_read(ulpi, reg); \
	if (ret < 0) \
		return ret; \
	seq_printf(seq, name " %.02x\n", ret); \
} while (0)

	ulpi_print("Vendor ID Low               ", ULPI_VENDOR_ID_LOW);
	ulpi_print("Vendor ID High              ", ULPI_VENDOR_ID_HIGH);
	ulpi_print("Product ID Low              ", ULPI_PRODUCT_ID_LOW);
	ulpi_print("Product ID High             ", ULPI_PRODUCT_ID_HIGH);
	ulpi_print("Function Control            ", ULPI_FUNC_CTRL);
	ulpi_print("Interface Control           ", ULPI_IFC_CTRL);
	ulpi_print("OTG Control                 ", ULPI_OTG_CTRL);
	ulpi_print("USB Interrupt Enable Rising ", ULPI_USB_INT_EN_RISE);
	ulpi_print("USB Interrupt Enable Falling", ULPI_USB_INT_EN_FALL);
	ulpi_print("USB Interrupt Status        ", ULPI_USB_INT_STS);
	ulpi_print("USB Interrupt Latch         ", ULPI_USB_INT_LATCH);
	ulpi_print("Debug                       ", ULPI_DEBUG);
	ulpi_print("Scratch Register            ", ULPI_SCRATCH);
	ulpi_print("Carkit Control              ", ULPI_CARKIT_CTRL);
	ulpi_print("Carkit Interrupt Delay      ", ULPI_CARKIT_INT_DELAY);
	ulpi_print("Carkit Interrupt Enable     ", ULPI_CARKIT_INT_EN);
	ulpi_print("Carkit Interrupt Status     ", ULPI_CARKIT_INT_STS);
	ulpi_print("Carkit Interrupt Latch      ", ULPI_CARKIT_INT_LATCH);
	ulpi_print("Carkit Pulse Control        ", ULPI_CARKIT_PLS_CTRL);
	ulpi_print("Transmit Positive Width     ", ULPI_TX_POS_WIDTH);
	ulpi_print("Transmit Negative Width     ", ULPI_TX_NEG_WIDTH);
	ulpi_print("Receive Polarity Recovery   ", ULPI_POLARITY_RECOVERY);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ulpi_regs);

static struct dentry *ulpi_root;

static int ulpi_register(struct device *dev, struct ulpi *ulpi)
{
	int ret;
	struct dentry *root;

	ulpi->dev.parent = dev; /* needed early for ops */
	ulpi->dev.bus = &ulpi_bus;
	ulpi->dev.type = &ulpi_dev_type;
	dev_set_name(&ulpi->dev, "%s.ulpi", dev_name(dev));

	ACPI_COMPANION_SET(&ulpi->dev, ACPI_COMPANION(dev));

	ret = ulpi_of_register(ulpi);
	if (ret)
		return ret;

	ret = ulpi_read_id(ulpi);
	if (ret) {
		of_node_put(ulpi->dev.of_node);
		return ret;
	}

	ret = device_register(&ulpi->dev);
	if (ret) {
		put_device(&ulpi->dev);
		return ret;
	}

	root = debugfs_create_dir(dev_name(dev), ulpi_root);
	debugfs_create_file("regs", 0444, root, ulpi, &ulpi_regs_fops);

	dev_dbg(&ulpi->dev, "registered ULPI PHY: vendor %04x, product %04x\n",
		ulpi->id.vendor, ulpi->id.product);

	return 0;
}

/**
 * ulpi_register_interface - instantiate new ULPI device
 * @dev: USB controller's device interface
 * @ops: ULPI register access
 *
 * Allocates and registers a ULPI device and an interface for it. Called from
 * the USB controller that provides the ULPI interface.
 */
struct ulpi *ulpi_register_interface(struct device *dev,
				     const struct ulpi_ops *ops)
{
	struct ulpi *ulpi;
	int ret;

	ulpi = kzalloc(sizeof(*ulpi), GFP_KERNEL);
	if (!ulpi)
		return ERR_PTR(-ENOMEM);

	ulpi->ops = ops;

	ret = ulpi_register(dev, ulpi);
	if (ret) {
		kfree(ulpi);
		return ERR_PTR(ret);
	}

	return ulpi;
}
EXPORT_SYMBOL_GPL(ulpi_register_interface);

/**
 * ulpi_unregister_interface - unregister ULPI interface
 * @ulpi: struct ulpi_interface
 *
 * Unregisters a ULPI device and it's interface that was created with
 * ulpi_create_interface().
 */
void ulpi_unregister_interface(struct ulpi *ulpi)
{
	debugfs_lookup_and_remove(dev_name(&ulpi->dev), ulpi_root);
	device_unregister(&ulpi->dev);
}
EXPORT_SYMBOL_GPL(ulpi_unregister_interface);

/* -------------------------------------------------------------------------- */

static int __init ulpi_init(void)
{
	int ret;

	ulpi_root = debugfs_create_dir(KBUILD_MODNAME, NULL);
	ret = bus_register(&ulpi_bus);
	if (ret)
		debugfs_remove(ulpi_root);
	return ret;
}
subsys_initcall(ulpi_init);

static void __exit ulpi_exit(void)
{
	bus_unregister(&ulpi_bus);
	debugfs_remove(ulpi_root);
}
module_exit(ulpi_exit);

MODULE_AUTHOR("Intel Corporation");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("USB ULPI PHY bus");
