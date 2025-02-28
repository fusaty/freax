// SPDX-License-Identifier: GPL-2.0-only
/*
 * Broadcom Northstar USB 2.0 PHY Driver
 *
 * Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
 */

#include <freax/bcma/bcma.h>
#include <freax/clk.h>
#include <freax/delay.h>
#include <freax/err.h>
#include <freax/mfd/syscon.h>
#include <freax/module.h>
#include <freax/of_address.h>
#include <freax/of_platform.h>
#include <freax/phy/phy.h>
#include <freax/platform_device.h>
#include <freax/regmap.h>
#include <freax/slab.h>

struct bcm_ns_usb2 {
	struct device *dev;
	struct clk *ref_clk;
	struct phy *phy;
	struct regmap *clkset;
	void __iomem *base;

	/* Deprecated binding */
	void __iomem *dmu;
};

static int bcm_ns_usb2_phy_init(struct phy *phy)
{
	struct bcm_ns_usb2 *usb2 = phy_get_drvdata(phy);
	struct device *dev = usb2->dev;
	u32 ref_clk_rate, usb2ctl, usb_pll_ndiv, usb_pll_pdiv;
	int err = 0;

	err = clk_prepare_enable(usb2->ref_clk);
	if (err < 0) {
		dev_err(dev, "Failed to prepare ref clock: %d\n", err);
		goto err_out;
	}

	ref_clk_rate = clk_get_rate(usb2->ref_clk);
	if (!ref_clk_rate) {
		dev_err(dev, "Failed to get ref clock rate\n");
		err = -EINVAL;
		goto err_clk_off;
	}

	if (usb2->base)
		usb2ctl = readl(usb2->base);
	else
		usb2ctl = readl(usb2->dmu + BCMA_DMU_CRU_USB2_CONTROL);

	if (usb2ctl & BCMA_DMU_CRU_USB2_CONTROL_USB_PLL_PDIV_MASK) {
		usb_pll_pdiv = usb2ctl;
		usb_pll_pdiv &= BCMA_DMU_CRU_USB2_CONTROL_USB_PLL_PDIV_MASK;
		usb_pll_pdiv >>= BCMA_DMU_CRU_USB2_CONTROL_USB_PLL_PDIV_SHIFT;
	} else {
		usb_pll_pdiv = 1 << 3;
	}

	/* Calculate ndiv based on a solid 1920 MHz that is for USB2 PHY */
	usb_pll_ndiv = (1920000000 * usb_pll_pdiv) / ref_clk_rate;

	/* Unlock DMU PLL settings with some magic value */
	if (usb2->clkset)
		regmap_write(usb2->clkset, 0, 0x0000ea68);
	else
		writel(0x0000ea68, usb2->dmu + BCMA_DMU_CRU_CLKSET_KEY);

	/* Write USB 2.0 PLL control setting */
	usb2ctl &= ~BCMA_DMU_CRU_USB2_CONTROL_USB_PLL_NDIV_MASK;
	usb2ctl |= usb_pll_ndiv << BCMA_DMU_CRU_USB2_CONTROL_USB_PLL_NDIV_SHIFT;
	if (usb2->base)
		writel(usb2ctl, usb2->base);
	else
		writel(usb2ctl, usb2->dmu + BCMA_DMU_CRU_USB2_CONTROL);

	/* Lock DMU PLL settings */
	if (usb2->clkset)
		regmap_write(usb2->clkset, 0, 0x00000000);
	else
		writel(0x00000000, usb2->dmu + BCMA_DMU_CRU_CLKSET_KEY);

err_clk_off:
	clk_disable_unprepare(usb2->ref_clk);
err_out:
	return err;
}

static const struct phy_ops ops = {
	.init		= bcm_ns_usb2_phy_init,
	.owner		= THIS_MODULE,
};

static int bcm_ns_usb2_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm_ns_usb2 *usb2;
	struct phy_provider *phy_provider;

	usb2 = devm_kzalloc(&pdev->dev, sizeof(*usb2), GFP_KERNEL);
	if (!usb2)
		return -ENOMEM;
	usb2->dev = dev;

	if (of_property_present(dev->of_node, "brcm,syscon-clkset")) {
		usb2->base = devm_platform_ioremap_resource(pdev, 0);
		if (IS_ERR(usb2->base)) {
			dev_err(dev, "Failed to map control reg\n");
			return PTR_ERR(usb2->base);
		}

		usb2->clkset = syscon_regmap_lookup_by_phandle(dev->of_node,
							       "brcm,syscon-clkset");
		if (IS_ERR(usb2->clkset)) {
			dev_err(dev, "Failed to lookup clkset regmap\n");
			return PTR_ERR(usb2->clkset);
		}
	} else {
		usb2->dmu = devm_platform_ioremap_resource_byname(pdev, "dmu");
		if (IS_ERR(usb2->dmu)) {
			dev_err(dev, "Failed to map DMU regs\n");
			return PTR_ERR(usb2->dmu);
		}

		dev_warn(dev, "using deprecated DT binding\n");
	}

	usb2->ref_clk = devm_clk_get(dev, "phy-ref-clk");
	if (IS_ERR(usb2->ref_clk)) {
		dev_err_probe(dev, PTR_ERR(usb2->ref_clk), "failed to get ref clk\n");
		return PTR_ERR(usb2->ref_clk);
	}

	usb2->phy = devm_phy_create(dev, NULL, &ops);
	if (IS_ERR(usb2->phy))
		return PTR_ERR(usb2->phy);

	phy_set_drvdata(usb2->phy, usb2);
	platform_set_drvdata(pdev, usb2);

	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	return PTR_ERR_OR_ZERO(phy_provider);
}

static const struct of_device_id bcm_ns_usb2_id_table[] = {
	{ .compatible = "brcm,ns-usb2-phy", },
	{},
};
MODULE_DEVICE_TABLE(of, bcm_ns_usb2_id_table);

static struct platform_driver bcm_ns_usb2_driver = {
	.probe		= bcm_ns_usb2_probe,
	.driver = {
		.name = "bcm_ns_usb2",
		.of_match_table = bcm_ns_usb2_id_table,
	},
};
module_platform_driver(bcm_ns_usb2_driver);

MODULE_LICENSE("GPL v2");
