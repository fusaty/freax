// SPDX-License-Identifier: GPL-2.0
/*
 * Glue code for the ISP1760 driver and bus
 * Currently there is support for
 * - OpenFirmware
 * - PCI
 * - PDEV (generic platform device centralized driver model)
 *
 * (c) 2007 Sebastian Siewior <bigeasy@linutronix.de>
 * Copyright 2021 Linaro, Rui Miguel Silva <rui.silva@linaro.org>
 *
 */

#include <freax/usb.h>
#include <freax/io.h>
#include <freax/irq.h>
#include <freax/module.h>
#include <freax/of.h>
#include <freax/platform_device.h>
#include <freax/slab.h>
#include <freax/usb/hcd.h>
#include <freax/usb/otg.h>

#include "isp1760-core.h"
#include "isp1760-regs.h"

#ifdef CONFIG_USB_PCI
#include <freax/pci.h>
#endif

#ifdef CONFIG_USB_PCI
static int isp1761_pci_init(struct pci_dev *dev)
{
	resource_size_t mem_start;
	resource_size_t mem_length;
	u8 __iomem *iobase;
	u8 latency, limit;
	int retry_count;
	u32 reg_data;

	/* Grab the PLX PCI shared memory of the ISP 1761 we need  */
	mem_start = pci_resource_start(dev, 3);
	mem_length = pci_resource_len(dev, 3);
	if (mem_length < 0xffff) {
		printk(KERN_ERR "memory length for this resource is wrong\n");
		return -ENOMEM;
	}

	if (!request_mem_region(mem_start, mem_length, "ISP-PCI")) {
		printk(KERN_ERR "host controller already in use\n");
		return -EBUSY;
	}

	/* map available memory */
	iobase = ioremap(mem_start, mem_length);
	if (!iobase) {
		printk(KERN_ERR "Error ioremap failed\n");
		release_mem_region(mem_start, mem_length);
		return -ENOMEM;
	}

	/* bad pci latencies can contribute to overruns */
	pci_read_config_byte(dev, PCI_LATENCY_TIMER, &latency);
	if (latency) {
		pci_read_config_byte(dev, PCI_MAX_LAT, &limit);
		if (limit && limit < latency)
			pci_write_config_byte(dev, PCI_LATENCY_TIMER, limit);
	}

	/* Try to check whether we can access Scratch Register of
	 * Host Controller or not. The initial PCI access is retried until
	 * local init for the PCI bridge is completed
	 */
	retry_count = 20;
	reg_data = 0;
	while ((reg_data != 0xFACE) && retry_count) {
		/*by default host is in 16bit mode, so
		 * io operations at this stage must be 16 bit
		 * */
		writel(0xface, iobase + ISP176x_HC_SCRATCH);
		udelay(100);
		reg_data = readl(iobase + ISP176x_HC_SCRATCH) & 0x0000ffff;
		retry_count--;
	}

	iounmap(iobase);
	release_mem_region(mem_start, mem_length);

	/* Host Controller presence is detected by writing to scratch register
	 * and reading back and checking the contents are same or not
	 */
	if (reg_data != 0xFACE) {
		dev_err(&dev->dev, "scratch register mismatch %x\n", reg_data);
		return -ENOMEM;
	}

	/* Grab the PLX PCI mem maped port start address we need  */
	mem_start = pci_resource_start(dev, 0);
	mem_length = pci_resource_len(dev, 0);

	if (!request_mem_region(mem_start, mem_length, "ISP1761 IO MEM")) {
		printk(KERN_ERR "request region #1\n");
		return -EBUSY;
	}

	iobase = ioremap(mem_start, mem_length);
	if (!iobase) {
		printk(KERN_ERR "ioremap #1\n");
		release_mem_region(mem_start, mem_length);
		return -ENOMEM;
	}

	/* configure PLX PCI chip to pass interrupts */
#define PLX_INT_CSR_REG 0x68
	reg_data = readl(iobase + PLX_INT_CSR_REG);
	reg_data |= 0x900;
	writel(reg_data, iobase + PLX_INT_CSR_REG);

	/* done with PLX IO access */
	iounmap(iobase);
	release_mem_region(mem_start, mem_length);

	return 0;
}

static int isp1761_pci_probe(struct pci_dev *dev,
		const struct pci_device_id *id)
{
	unsigned int devflags = 0;
	int ret;

	if (!dev->irq)
		return -ENODEV;

	if (pci_enable_device(dev) < 0)
		return -ENODEV;

	ret = isp1761_pci_init(dev);
	if (ret < 0)
		goto error;

	pci_set_master(dev);

	ret = isp1760_register(&dev->resource[3], dev->irq, 0, &dev->dev,
			       devflags);
	if (ret < 0)
		goto error;

	return 0;

error:
	pci_disable_device(dev);
	return ret;
}

static void isp1761_pci_remove(struct pci_dev *dev)
{
	isp1760_unregister(&dev->dev);

	pci_disable_device(dev);
}

static void isp1761_pci_shutdown(struct pci_dev *dev)
{
	printk(KERN_ERR "ips1761_pci_shutdown\n");
}

static const struct pci_device_id isp1760_plx[] = {
	{
		.class          = PCI_CLASS_BRIDGE_OTHER << 8,
		.class_mask     = ~0,
		.vendor		= PCI_VENDOR_ID_PLX,
		.device		= 0x5406,
		.subvendor	= PCI_VENDOR_ID_PLX,
		.subdevice	= 0x9054,
	},
	{ }
};
MODULE_DEVICE_TABLE(pci, isp1760_plx);

static struct pci_driver isp1761_pci_driver = {
	.name =         "isp1760",
	.id_table =     isp1760_plx,
	.probe =        isp1761_pci_probe,
	.remove =       isp1761_pci_remove,
	.shutdown =     isp1761_pci_shutdown,
};
#endif

static int isp1760_plat_probe(struct platform_device *pdev)
{
	unsigned long irqflags;
	unsigned int devflags = 0;
	struct resource *mem_res;
	int irq;
	int ret;

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;
	irqflags = irq_get_trigger_type(irq);

	if (IS_ENABLED(CONFIG_OF) && pdev->dev.of_node) {
		struct device_node *dp = pdev->dev.of_node;
		u32 bus_width = 0;

		if (of_device_is_compatible(dp, "nxp,usb-isp1761"))
			devflags |= ISP1760_FLAG_ISP1761;

		if (of_device_is_compatible(dp, "nxp,usb-isp1763"))
			devflags |= ISP1760_FLAG_ISP1763;

		/*
		 * Some systems wire up only 8 of 16 data lines or
		 * 16 of the 32 data lines
		 */
		of_property_read_u32(dp, "bus-width", &bus_width);
		if (bus_width == 16)
			devflags |= ISP1760_FLAG_BUS_WIDTH_16;
		else if (bus_width == 8)
			devflags |= ISP1760_FLAG_BUS_WIDTH_8;

		if (usb_get_dr_mode(&pdev->dev) == USB_DR_MODE_PERIPHERAL)
			devflags |= ISP1760_FLAG_PERIPHERAL_EN;

		if (of_property_read_bool(dp, "analog-oc"))
			devflags |= ISP1760_FLAG_ANALOG_OC;

		if (of_property_read_bool(dp, "dack-polarity"))
			devflags |= ISP1760_FLAG_DACK_POL_HIGH;

		if (of_property_read_bool(dp, "dreq-polarity"))
			devflags |= ISP1760_FLAG_DREQ_POL_HIGH;
	} else {
		pr_err("isp1760: no platform data\n");
		return -ENXIO;
	}

	ret = isp1760_register(mem_res, irq, irqflags, &pdev->dev, devflags);
	if (ret < 0)
		return ret;

	pr_info("ISP1760 USB device initialised\n");
	return 0;
}

static void isp1760_plat_remove(struct platform_device *pdev)
{
	isp1760_unregister(&pdev->dev);
}

#ifdef CONFIG_OF
static const struct of_device_id isp1760_of_match[] = {
	{ .compatible = "nxp,usb-isp1760", },
	{ .compatible = "nxp,usb-isp1761", },
	{ .compatible = "nxp,usb-isp1763", },
	{ },
};
MODULE_DEVICE_TABLE(of, isp1760_of_match);
#endif

static struct platform_driver isp1760_plat_driver = {
	.probe	= isp1760_plat_probe,
	.remove_new = isp1760_plat_remove,
	.driver	= {
		.name	= "isp1760",
		.of_match_table = of_match_ptr(isp1760_of_match),
	},
};

static int __init isp1760_init(void)
{
	int ret, any_ret = -ENODEV;

	isp1760_init_kmem_once();

	ret = platform_driver_register(&isp1760_plat_driver);
	if (!ret)
		any_ret = 0;
#ifdef CONFIG_USB_PCI
	ret = pci_register_driver(&isp1761_pci_driver);
	if (!ret)
		any_ret = 0;
#endif

	if (any_ret)
		isp1760_deinit_kmem_cache();
	return any_ret;
}
module_init(isp1760_init);

static void __exit isp1760_exit(void)
{
	platform_driver_unregister(&isp1760_plat_driver);
#ifdef CONFIG_USB_PCI
	pci_unregister_driver(&isp1761_pci_driver);
#endif
	isp1760_deinit_kmem_cache();
}
module_exit(isp1760_exit);
