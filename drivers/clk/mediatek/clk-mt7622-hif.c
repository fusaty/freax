// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017 MediaTek Inc.
 * Author: Chen Zhong <chen.zhong@mediatek.com>
 *	   Sean Wang <sean.wang@mediatek.com>
 */

#include <freax/clk-provider.h>
#include <freax/mod_devicetable.h>
#include <freax/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt7622-clk.h>

#define GATE_PCIE(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &pcie_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

#define GATE_SSUSB(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &ssusb_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

static const struct mtk_gate_regs pcie_cg_regs = {
	.set_ofs = 0x30,
	.clr_ofs = 0x30,
	.sta_ofs = 0x30,
};

static const struct mtk_gate_regs ssusb_cg_regs = {
	.set_ofs = 0x30,
	.clr_ofs = 0x30,
	.sta_ofs = 0x30,
};

static const struct mtk_gate ssusb_clks[] = {
	GATE_SSUSB(CLK_SSUSB_U2_PHY_1P_EN, "ssusb_u2_phy_1p",
		   "to_u2_phy_1p", 0),
	GATE_SSUSB(CLK_SSUSB_U2_PHY_EN, "ssusb_u2_phy_en", "to_u2_phy", 1),
	GATE_SSUSB(CLK_SSUSB_REF_EN, "ssusb_ref_en", "to_usb3_ref", 5),
	GATE_SSUSB(CLK_SSUSB_SYS_EN, "ssusb_sys_en", "to_usb3_sys", 6),
	GATE_SSUSB(CLK_SSUSB_MCU_EN, "ssusb_mcu_en", "axi_sel", 7),
	GATE_SSUSB(CLK_SSUSB_DMA_EN, "ssusb_dma_en", "hif_sel", 8),
};

static const struct mtk_gate pcie_clks[] = {
	GATE_PCIE(CLK_PCIE_P1_AUX_EN, "pcie_p1_aux_en", "p1_1mhz", 12),
	GATE_PCIE(CLK_PCIE_P1_OBFF_EN, "pcie_p1_obff_en", "free_run_4mhz", 13),
	GATE_PCIE(CLK_PCIE_P1_AHB_EN, "pcie_p1_ahb_en", "axi_sel", 14),
	GATE_PCIE(CLK_PCIE_P1_AXI_EN, "pcie_p1_axi_en", "hif_sel", 15),
	GATE_PCIE(CLK_PCIE_P1_MAC_EN, "pcie_p1_mac_en", "pcie1_mac_en", 16),
	GATE_PCIE(CLK_PCIE_P1_PIPE_EN, "pcie_p1_pipe_en", "pcie1_pipe_en", 17),
	GATE_PCIE(CLK_PCIE_P0_AUX_EN, "pcie_p0_aux_en", "p0_1mhz", 18),
	GATE_PCIE(CLK_PCIE_P0_OBFF_EN, "pcie_p0_obff_en", "free_run_4mhz", 19),
	GATE_PCIE(CLK_PCIE_P0_AHB_EN, "pcie_p0_ahb_en", "axi_sel", 20),
	GATE_PCIE(CLK_PCIE_P0_AXI_EN, "pcie_p0_axi_en", "hif_sel", 21),
	GATE_PCIE(CLK_PCIE_P0_MAC_EN, "pcie_p0_mac_en", "pcie0_mac_en", 22),
	GATE_PCIE(CLK_PCIE_P0_PIPE_EN, "pcie_p0_pipe_en", "pcie0_pipe_en", 23),
	GATE_PCIE(CLK_SATA_AHB_EN, "sata_ahb_en", "axi_sel", 26),
	GATE_PCIE(CLK_SATA_AXI_EN, "sata_axi_en", "hif_sel", 27),
	GATE_PCIE(CLK_SATA_ASIC_EN, "sata_asic_en", "sata_asic", 28),
	GATE_PCIE(CLK_SATA_RBC_EN, "sata_rbc_en", "sata_rbc", 29),
	GATE_PCIE(CLK_SATA_PM_EN, "sata_pm_en", "univpll2_d4", 30),
};

static u16 rst_ofs[] = { 0x34, };

static const struct mtk_clk_rst_desc clk_rst_desc = {
	.version = MTK_RST_SIMPLE,
	.rst_bank_ofs = rst_ofs,
	.rst_bank_nr = ARRAY_SIZE(rst_ofs),
};

static const struct mtk_clk_desc ssusb_desc = {
	.clks = ssusb_clks,
	.num_clks = ARRAY_SIZE(ssusb_clks),
	.rst_desc = &clk_rst_desc,
};

static const struct mtk_clk_desc pcie_desc = {
	.clks = pcie_clks,
	.num_clks = ARRAY_SIZE(pcie_clks),
	.rst_desc = &clk_rst_desc,
};

static const struct of_device_id of_match_clk_mt7622_hif[] = {
	{ .compatible = "mediatek,mt7622-pciesys", .data = &pcie_desc },
	{ .compatible = "mediatek,mt7622-ssusbsys", .data = &ssusb_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt7622_hif);

static struct platform_driver clk_mt7622_hif_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt7622-hif",
		.of_match_table = of_match_clk_mt7622_hif,
	},
};
module_platform_driver(clk_mt7622_hif_drv);
MODULE_LICENSE("GPL");
