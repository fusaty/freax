// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016, NVIDIA Corporation
 */

#include <freax/clk.h>
#include <freax/io.h>
#include <freax/module.h>
#include <freax/of.h>
#include <freax/platform_device.h>
#include <freax/pm_runtime.h>
#include <freax/reset.h>

#include <freax/usb.h>
#include <freax/usb/chipidea.h>
#include <freax/usb/hcd.h>
#include <freax/usb/of.h>
#include <freax/usb/phy.h>

#include <soc/tegra/common.h>

#include "../host/ehci.h"

#include "ci.h"

struct tegra_usb {
	struct ci_hdrc_platform_data data;
	struct platform_device *dev;

	const struct tegra_usb_soc_info *soc;
	struct usb_phy *phy;
	struct clk *clk;

	bool needs_double_reset;
};

struct tegra_usb_soc_info {
	unsigned long flags;
	unsigned int txfifothresh;
	enum usb_dr_mode dr_mode;
};

static const struct tegra_usb_soc_info tegra20_ehci_soc_info = {
	.flags = CI_HDRC_REQUIRES_ALIGNED_DMA |
		 CI_HDRC_OVERRIDE_PHY_CONTROL |
		 CI_HDRC_SUPPORTS_RUNTIME_PM,
	.dr_mode = USB_DR_MODE_HOST,
	.txfifothresh = 10,
};

static const struct tegra_usb_soc_info tegra30_ehci_soc_info = {
	.flags = CI_HDRC_REQUIRES_ALIGNED_DMA |
		 CI_HDRC_OVERRIDE_PHY_CONTROL |
		 CI_HDRC_SUPPORTS_RUNTIME_PM,
	.dr_mode = USB_DR_MODE_HOST,
	.txfifothresh = 16,
};

static const struct tegra_usb_soc_info tegra20_udc_soc_info = {
	.flags = CI_HDRC_REQUIRES_ALIGNED_DMA |
		 CI_HDRC_OVERRIDE_PHY_CONTROL |
		 CI_HDRC_SUPPORTS_RUNTIME_PM,
	.dr_mode = USB_DR_MODE_UNKNOWN,
	.txfifothresh = 10,
};

static const struct tegra_usb_soc_info tegra30_udc_soc_info = {
	.flags = CI_HDRC_REQUIRES_ALIGNED_DMA |
		 CI_HDRC_OVERRIDE_PHY_CONTROL |
		 CI_HDRC_SUPPORTS_RUNTIME_PM,
	.dr_mode = USB_DR_MODE_UNKNOWN,
	.txfifothresh = 16,
};

static const struct of_device_id tegra_usb_of_match[] = {
	{
		.compatible = "nvidia,tegra20-ehci",
		.data = &tegra20_ehci_soc_info,
	}, {
		.compatible = "nvidia,tegra30-ehci",
		.data = &tegra30_ehci_soc_info,
	}, {
		.compatible = "nvidia,tegra20-udc",
		.data = &tegra20_udc_soc_info,
	}, {
		.compatible = "nvidia,tegra30-udc",
		.data = &tegra30_udc_soc_info,
	}, {
		.compatible = "nvidia,tegra114-udc",
		.data = &tegra30_udc_soc_info,
	}, {
		.compatible = "nvidia,tegra124-udc",
		.data = &tegra30_udc_soc_info,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, tegra_usb_of_match);

static int tegra_usb_reset_controller(struct device *dev)
{
	struct reset_control *rst, *rst_utmi;
	struct device_node *phy_np;
	int err;

	rst = devm_reset_control_get_shared(dev, "usb");
	if (IS_ERR(rst)) {
		dev_err(dev, "can't get ehci reset: %pe\n", rst);
		return PTR_ERR(rst);
	}

	phy_np = of_parse_phandle(dev->of_node, "nvidia,phy", 0);
	if (!phy_np)
		return -ENOENT;

	/*
	 * The 1st USB controller contains some UTMI pad registers that are
	 * global for all the controllers on the chip. Those registers are
	 * also cleared when reset is asserted to the 1st controller.
	 */
	rst_utmi = of_reset_control_get_shared(phy_np, "utmi-pads");
	if (IS_ERR(rst_utmi)) {
		dev_warn(dev, "can't get utmi-pads reset from the PHY\n");
		dev_warn(dev, "continuing, but please update your DT\n");
	} else {
		/*
		 * PHY driver performs UTMI-pads reset in a case of a
		 * non-legacy DT.
		 */
		reset_control_put(rst_utmi);
	}

	of_node_put(phy_np);

	/* reset control is shared, hence initialize it first */
	err = reset_control_deassert(rst);
	if (err)
		return err;

	err = reset_control_assert(rst);
	if (err)
		return err;

	udelay(1);

	err = reset_control_deassert(rst);
	if (err)
		return err;

	return 0;
}

static int tegra_usb_notify_event(struct ci_hdrc *ci, unsigned int event)
{
	struct tegra_usb *usb = dev_get_drvdata(ci->dev->parent);
	struct ehci_hcd *ehci;

	switch (event) {
	case CI_HDRC_CONTROLLER_RESET_EVENT:
		if (ci->hcd) {
			ehci = hcd_to_ehci(ci->hcd);
			ehci->has_tdi_phy_lpm = false;
			ehci_writel(ehci, usb->soc->txfifothresh << 16,
				    &ehci->regs->txfill_tuning);
		}
		break;
	}

	return 0;
}

static int tegra_usb_internal_port_reset(struct ehci_hcd *ehci,
					 u32 __iomem *portsc_reg,
					 unsigned long *flags)
{
	u32 saved_usbintr, temp;
	unsigned int i, tries;
	int retval = 0;

	saved_usbintr = ehci_readl(ehci, &ehci->regs->intr_enable);
	/* disable USB interrupt */
	ehci_writel(ehci, 0, &ehci->regs->intr_enable);
	spin_unlock_irqrestore(&ehci->lock, *flags);

	/*
	 * Here we have to do Port Reset at most twice for
	 * Port Enable bit to be set.
	 */
	for (i = 0; i < 2; i++) {
		temp = ehci_readl(ehci, portsc_reg);
		temp |= PORT_RESET;
		ehci_writel(ehci, temp, portsc_reg);
		fsleep(10000);
		temp &= ~PORT_RESET;
		ehci_writel(ehci, temp, portsc_reg);
		fsleep(1000);
		tries = 100;
		do {
			fsleep(1000);
			/*
			 * Up to this point, Port Enable bit is
			 * expected to be set after 2 ms waiting.
			 * USB1 usually takes extra 45 ms, for safety,
			 * we take 100 ms as timeout.
			 */
			temp = ehci_readl(ehci, portsc_reg);
		} while (!(temp & PORT_PE) && tries--);
		if (temp & PORT_PE)
			break;
	}
	if (i == 2)
		retval = -ETIMEDOUT;

	/*
	 * Clear Connect Status Change bit if it's set.
	 * We can't clear PORT_PEC. It will also cause PORT_PE to be cleared.
	 */
	if (temp & PORT_CSC)
		ehci_writel(ehci, PORT_CSC, portsc_reg);

	/*
	 * Write to clear any interrupt status bits that might be set
	 * during port reset.
	 */
	temp = ehci_readl(ehci, &ehci->regs->status);
	ehci_writel(ehci, temp, &ehci->regs->status);

	/* restore original interrupt-enable bits */
	spin_lock_irqsave(&ehci->lock, *flags);
	ehci_writel(ehci, saved_usbintr, &ehci->regs->intr_enable);

	return retval;
}

static int tegra_ehci_hub_control(struct ci_hdrc *ci, u16 typeReq, u16 wValue,
				  u16 wIndex, char *buf, u16 wLength,
				  bool *done, unsigned long *flags)
{
	struct tegra_usb *usb = dev_get_drvdata(ci->dev->parent);
	struct ehci_hcd *ehci = hcd_to_ehci(ci->hcd);
	u32 __iomem *status_reg;
	int retval = 0;

	status_reg = &ehci->regs->port_status[(wIndex & 0xff) - 1];

	switch (typeReq) {
	case SetPortFeature:
		if (wValue != USB_PORT_FEAT_RESET || !usb->needs_double_reset)
			break;

		/* for USB1 port we need to issue Port Reset twice internally */
		retval = tegra_usb_internal_port_reset(ehci, status_reg, flags);
		*done  = true;
		break;
	}

	return retval;
}

static void tegra_usb_enter_lpm(struct ci_hdrc *ci, bool enable)
{
	/*
	 * Touching any register which belongs to AHB clock domain will
	 * hang CPU if USB controller is put into low power mode because
	 * AHB USB clock is gated on Tegra in the LPM.
	 *
	 * Tegra PHY has a separate register for checking the clock status
	 * and usb_phy_set_suspend() takes care of gating/ungating the clocks
	 * and restoring the PHY state on Tegra. Hence DEVLC/PORTSC registers
	 * shouldn't be touched directly by the CI driver.
	 */
	usb_phy_set_suspend(ci->usb_phy, enable);
}

static int tegra_usb_probe(struct platform_device *pdev)
{
	const struct tegra_usb_soc_info *soc;
	struct tegra_usb *usb;
	int err;

	usb = devm_kzalloc(&pdev->dev, sizeof(*usb), GFP_KERNEL);
	if (!usb)
		return -ENOMEM;

	platform_set_drvdata(pdev, usb);

	soc = of_device_get_match_data(&pdev->dev);
	if (!soc) {
		dev_err(&pdev->dev, "failed to match OF data\n");
		return -EINVAL;
	}

	usb->phy = devm_usb_get_phy_by_phandle(&pdev->dev, "nvidia,phy", 0);
	if (IS_ERR(usb->phy))
		return dev_err_probe(&pdev->dev, PTR_ERR(usb->phy),
				     "failed to get PHY");

	usb->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(usb->clk))
		return dev_err_probe(&pdev->dev, PTR_ERR(usb->clk),
				     "failed to get clock");

	err = devm_tegra_core_dev_init_opp_table_common(&pdev->dev);
	if (err)
		return err;

	pm_runtime_enable(&pdev->dev);
	err = pm_runtime_resume_and_get(&pdev->dev);
	if (err)
		return err;

	if (device_property_present(&pdev->dev, "nvidia,needs-double-reset"))
		usb->needs_double_reset = true;

	err = tegra_usb_reset_controller(&pdev->dev);
	if (err) {
		dev_err_probe(&pdev->dev, err, "failed to reset controller");
		goto fail_power_off;
	}

	/*
	 * USB controller registers shouldn't be touched before PHY is
	 * initialized, otherwise CPU will hang because clocks are gated.
	 * PHY driver controls gating of internal USB clocks on Tegra.
	 */
	err = usb_phy_init(usb->phy);
	if (err)
		goto fail_power_off;

	/* setup and register ChipIdea HDRC device */
	usb->soc = soc;
	usb->data.name = "tegra-usb";
	usb->data.flags = soc->flags;
	usb->data.usb_phy = usb->phy;
	usb->data.dr_mode = soc->dr_mode;
	usb->data.capoffset = DEF_CAPOFFSET;
	usb->data.enter_lpm = tegra_usb_enter_lpm;
	usb->data.hub_control = tegra_ehci_hub_control;
	usb->data.notify_event = tegra_usb_notify_event;

	/* Tegra PHY driver currently doesn't support LPM for ULPI */
	if (of_usb_get_phy_mode(pdev->dev.of_node) == USBPHY_INTERFACE_MODE_ULPI)
		usb->data.flags &= ~CI_HDRC_SUPPORTS_RUNTIME_PM;

	usb->dev = ci_hdrc_add_device(&pdev->dev, pdev->resource,
				      pdev->num_resources, &usb->data);
	if (IS_ERR(usb->dev)) {
		err = dev_err_probe(&pdev->dev, PTR_ERR(usb->dev),
				    "failed to add HDRC device");
		goto phy_shutdown;
	}

	return 0;

phy_shutdown:
	usb_phy_shutdown(usb->phy);
fail_power_off:
	pm_runtime_put_sync_suspend(&pdev->dev);
	pm_runtime_force_suspend(&pdev->dev);

	return err;
}

static void tegra_usb_remove(struct platform_device *pdev)
{
	struct tegra_usb *usb = platform_get_drvdata(pdev);

	ci_hdrc_remove_device(usb->dev);
	usb_phy_shutdown(usb->phy);

	pm_runtime_put_sync_suspend(&pdev->dev);
	pm_runtime_force_suspend(&pdev->dev);
}

static int __maybe_unused tegra_usb_runtime_resume(struct device *dev)
{
	struct tegra_usb *usb = dev_get_drvdata(dev);
	int err;

	err = clk_prepare_enable(usb->clk);
	if (err < 0) {
		dev_err(dev, "failed to enable clock: %d\n", err);
		return err;
	}

	return 0;
}

static int __maybe_unused tegra_usb_runtime_suspend(struct device *dev)
{
	struct tegra_usb *usb = dev_get_drvdata(dev);

	clk_disable_unprepare(usb->clk);

	return 0;
}

static const struct dev_pm_ops tegra_usb_pm = {
	SET_RUNTIME_PM_OPS(tegra_usb_runtime_suspend, tegra_usb_runtime_resume,
			   NULL)
};

static struct platform_driver tegra_usb_driver = {
	.driver = {
		.name = "tegra-usb",
		.of_match_table = tegra_usb_of_match,
		.pm = &tegra_usb_pm,
	},
	.probe = tegra_usb_probe,
	.remove_new = tegra_usb_remove,
};
module_platform_driver(tegra_usb_driver);

MODULE_DESCRIPTION("NVIDIA Tegra USB driver");
MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_LICENSE("GPL v2");
