// SPDX-License-Identifier: GPL-2.0+
/*
 * shmob_drm_drv.c  --  SH Mobile DRM driver
 *
 * Copyright (C) 2012 Renesas Electronics Corporation
 *
 * Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */

#include <freax/clk.h>
#include <freax/io.h>
#include <freax/mm.h>
#include <freax/module.h>
#include <freax/platform_device.h>
#include <freax/pm.h>
#include <freax/slab.h>

#include <drm/drm_drv.h>
#include <drm/drm_fbdev_generic.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_module.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_vblank.h>

#include "shmob_drm_drv.h"
#include "shmob_drm_kms.h"
#include "shmob_drm_plane.h"
#include "shmob_drm_regs.h"

/* -----------------------------------------------------------------------------
 * Hardware initialization
 */

static int shmob_drm_init_interface(struct shmob_drm_device *sdev)
{
	static const u32 ldmt1r[] = {
		[SHMOB_DRM_IFACE_RGB8] = LDMT1R_MIFTYP_RGB8,
		[SHMOB_DRM_IFACE_RGB9] = LDMT1R_MIFTYP_RGB9,
		[SHMOB_DRM_IFACE_RGB12A] = LDMT1R_MIFTYP_RGB12A,
		[SHMOB_DRM_IFACE_RGB12B] = LDMT1R_MIFTYP_RGB12B,
		[SHMOB_DRM_IFACE_RGB16] = LDMT1R_MIFTYP_RGB16,
		[SHMOB_DRM_IFACE_RGB18] = LDMT1R_MIFTYP_RGB18,
		[SHMOB_DRM_IFACE_RGB24] = LDMT1R_MIFTYP_RGB24,
		[SHMOB_DRM_IFACE_YUV422] = LDMT1R_MIFTYP_YCBCR,
		[SHMOB_DRM_IFACE_SYS8A] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS8A,
		[SHMOB_DRM_IFACE_SYS8B] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS8B,
		[SHMOB_DRM_IFACE_SYS8C] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS8C,
		[SHMOB_DRM_IFACE_SYS8D] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS8D,
		[SHMOB_DRM_IFACE_SYS9] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS9,
		[SHMOB_DRM_IFACE_SYS12] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS12,
		[SHMOB_DRM_IFACE_SYS16A] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS16A,
		[SHMOB_DRM_IFACE_SYS16B] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS16B,
		[SHMOB_DRM_IFACE_SYS16C] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS16C,
		[SHMOB_DRM_IFACE_SYS18] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS18,
		[SHMOB_DRM_IFACE_SYS24] = LDMT1R_IFM | LDMT1R_MIFTYP_SYS24,
	};

	if (sdev->pdata->iface.interface >= ARRAY_SIZE(ldmt1r)) {
		dev_err(sdev->dev, "invalid interface type %u\n",
			sdev->pdata->iface.interface);
		return -EINVAL;
	}

	sdev->ldmt1r = ldmt1r[sdev->pdata->iface.interface];
	return 0;
}

static int shmob_drm_setup_clocks(struct shmob_drm_device *sdev,
					    enum shmob_drm_clk_source clksrc)
{
	struct clk *clk;
	char *clkname;

	switch (clksrc) {
	case SHMOB_DRM_CLK_BUS:
		clkname = "bus_clk";
		sdev->lddckr = LDDCKR_ICKSEL_BUS;
		break;
	case SHMOB_DRM_CLK_PERIPHERAL:
		clkname = "peripheral_clk";
		sdev->lddckr = LDDCKR_ICKSEL_MIPI;
		break;
	case SHMOB_DRM_CLK_EXTERNAL:
		clkname = NULL;
		sdev->lddckr = LDDCKR_ICKSEL_HDMI;
		break;
	default:
		return -EINVAL;
	}

	clk = devm_clk_get(sdev->dev, clkname);
	if (IS_ERR(clk)) {
		dev_err(sdev->dev, "cannot get dot clock %s\n", clkname);
		return PTR_ERR(clk);
	}

	sdev->clock = clk;
	return 0;
}

/* -----------------------------------------------------------------------------
 * DRM operations
 */

static irqreturn_t shmob_drm_irq(int irq, void *arg)
{
	struct drm_device *dev = arg;
	struct shmob_drm_device *sdev = dev->dev_private;
	unsigned long flags;
	u32 status;

	/* Acknowledge interrupts. Putting interrupt enable and interrupt flag
	 * bits in the same register is really brain-dead design and requires
	 * taking a spinlock.
	 */
	spin_lock_irqsave(&sdev->irq_lock, flags);
	status = lcdc_read(sdev, LDINTR);
	lcdc_write(sdev, LDINTR, status ^ LDINTR_STATUS_MASK);
	spin_unlock_irqrestore(&sdev->irq_lock, flags);

	if (status & LDINTR_VES) {
		drm_handle_vblank(dev, 0);
		shmob_drm_crtc_finish_page_flip(&sdev->crtc);
	}

	return IRQ_HANDLED;
}

DEFINE_DRM_GEM_DMA_FOPS(shmob_drm_fops);

static const struct drm_driver shmob_drm_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET,
	DRM_GEM_DMA_DRIVER_OPS,
	.fops			= &shmob_drm_fops,
	.name			= "shmob-drm",
	.desc			= "Renesas SH Mobile DRM",
	.date			= "20120424",
	.major			= 1,
	.minor			= 0,
};

/* -----------------------------------------------------------------------------
 * Power management
 */

static int shmob_drm_pm_suspend(struct device *dev)
{
	struct shmob_drm_device *sdev = dev_get_drvdata(dev);

	drm_kms_helper_poll_disable(sdev->ddev);
	shmob_drm_crtc_suspend(&sdev->crtc);

	return 0;
}

static int shmob_drm_pm_resume(struct device *dev)
{
	struct shmob_drm_device *sdev = dev_get_drvdata(dev);

	drm_modeset_lock_all(sdev->ddev);
	shmob_drm_crtc_resume(&sdev->crtc);
	drm_modeset_unlock_all(sdev->ddev);

	drm_kms_helper_poll_enable(sdev->ddev);
	return 0;
}

static DEFINE_SIMPLE_DEV_PM_OPS(shmob_drm_pm_ops,
				shmob_drm_pm_suspend, shmob_drm_pm_resume);

/* -----------------------------------------------------------------------------
 * Platform driver
 */

static void shmob_drm_remove(struct platform_device *pdev)
{
	struct shmob_drm_device *sdev = platform_get_drvdata(pdev);
	struct drm_device *ddev = sdev->ddev;

	drm_dev_unregister(ddev);
	drm_kms_helper_poll_fini(ddev);
	free_irq(sdev->irq, ddev);
	drm_dev_put(ddev);
}

static int shmob_drm_probe(struct platform_device *pdev)
{
	struct shmob_drm_platform_data *pdata = pdev->dev.platform_data;
	struct shmob_drm_device *sdev;
	struct drm_device *ddev;
	unsigned int i;
	int ret;

	if (pdata == NULL) {
		dev_err(&pdev->dev, "no platform data\n");
		return -EINVAL;
	}

	/*
	 * Allocate and initialize the driver private data, I/O resources and
	 * clocks.
	 */
	sdev = devm_kzalloc(&pdev->dev, sizeof(*sdev), GFP_KERNEL);
	if (sdev == NULL)
		return -ENOMEM;

	sdev->dev = &pdev->dev;
	sdev->pdata = pdata;
	spin_lock_init(&sdev->irq_lock);

	platform_set_drvdata(pdev, sdev);

	sdev->mmio = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(sdev->mmio))
		return PTR_ERR(sdev->mmio);

	ret = shmob_drm_setup_clocks(sdev, pdata->clk_source);
	if (ret < 0)
		return ret;

	ret = shmob_drm_init_interface(sdev);
	if (ret < 0)
		return ret;

	/* Allocate and initialize the DRM device. */
	ddev = drm_dev_alloc(&shmob_drm_driver, &pdev->dev);
	if (IS_ERR(ddev))
		return PTR_ERR(ddev);

	sdev->ddev = ddev;
	ddev->dev_private = sdev;

	ret = shmob_drm_modeset_init(sdev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to initialize mode setting\n");
		goto err_free_drm_dev;
	}

	for (i = 0; i < 4; ++i) {
		ret = shmob_drm_plane_create(sdev, i);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to create plane %u\n", i);
			goto err_modeset_cleanup;
		}
	}

	ret = drm_vblank_init(ddev, 1);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to initialize vblank\n");
		goto err_modeset_cleanup;
	}

	ret = platform_get_irq(pdev, 0);
	if (ret < 0)
		goto err_modeset_cleanup;
	sdev->irq = ret;

	ret = request_irq(sdev->irq, shmob_drm_irq, 0, ddev->driver->name,
			  ddev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to install IRQ handler\n");
		goto err_modeset_cleanup;
	}

	/*
	 * Register the DRM device with the core and the connectors with
	 * sysfs.
	 */
	ret = drm_dev_register(ddev, 0);
	if (ret < 0)
		goto err_irq_uninstall;

	drm_fbdev_generic_setup(ddev, 16);

	return 0;

err_irq_uninstall:
	free_irq(sdev->irq, ddev);
err_modeset_cleanup:
	drm_kms_helper_poll_fini(ddev);
err_free_drm_dev:
	drm_dev_put(ddev);

	return ret;
}

static struct platform_driver shmob_drm_platform_driver = {
	.probe		= shmob_drm_probe,
	.remove_new	= shmob_drm_remove,
	.driver		= {
		.name	= "shmob-drm",
		.pm	= pm_sleep_ptr(&shmob_drm_pm_ops),
	},
};

drm_module_platform_driver(shmob_drm_platform_driver);

MODULE_AUTHOR("Laurent Pinchart <laurent.pinchart@ideasonboard.com>");
MODULE_DESCRIPTION("Renesas SH Mobile DRM Driver");
MODULE_LICENSE("GPL");
