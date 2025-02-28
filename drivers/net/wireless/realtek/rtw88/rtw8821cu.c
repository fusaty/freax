// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/* Copyright(c) 2018-2019  Realtek Corporation
 */

#include <freax/module.h>
#include <freax/usb.h>
#include "main.h"
#include "rtw8821c.h"
#include "usb.h"

static const struct usb_device_id rtw_8821cu_id_table[] = {
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xb82b, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xb820, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xc821, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xc820, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xc82a, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xc82b, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8821CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0xc811, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8811CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0x8811, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* 8811CU */
	{ USB_DEVICE_AND_INTERFACE_INFO(RTW_USB_VENDOR_ID_REALTEK, 0x2006, 0xff, 0xff, 0xff),
	  .driver_info = (kernel_ulong_t)&(rtw8821c_hw_spec) }, /* TOTOLINK A650UA v3 */
	{},
};
MODULE_DEVICE_TABLE(usb, rtw_8821cu_id_table);

static int rtw_8821cu_probe(struct usb_interface *intf,
			    const struct usb_device_id *id)
{
	return rtw_usb_probe(intf, id);
}

static struct usb_driver rtw_8821cu_driver = {
	.name = "rtw_8821cu",
	.id_table = rtw_8821cu_id_table,
	.probe = rtw_8821cu_probe,
	.disconnect = rtw_usb_disconnect,
};
module_usb_driver(rtw_8821cu_driver);

MODULE_AUTHOR("Hans Ulli Kroll <freax@ulli-kroll.de>");
MODULE_DESCRIPTION("Realtek 802.11ac wireless 8821cu driver");
MODULE_LICENSE("Dual BSD/GPL");
