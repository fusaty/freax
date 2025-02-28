// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017 MediaTek Inc.
 * Author: Kevin Chen <kevin-cw.chen@mediatek.com>
 */

#include <freax/clk-provider.h>
#include <freax/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt6797-clk.h>

static const struct mtk_gate_regs venc_cg_regs = {
	.set_ofs = 0x0004,
	.clr_ofs = 0x0008,
	.sta_ofs = 0x0000,
};

#define GATE_VENC(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &venc_cg_regs, _shift, &mtk_clk_gate_ops_setclr_inv)

static const struct mtk_gate venc_clks[] = {
	GATE_VENC(CLK_VENC_0, "venc_0", "mm_sel", 0),
	GATE_VENC(CLK_VENC_1, "venc_1", "venc_sel", 4),
	GATE_VENC(CLK_VENC_2, "venc_2", "venc_sel", 8),
	GATE_VENC(CLK_VENC_3, "venc_3", "venc_sel", 12),
};

static const struct mtk_clk_desc venc_desc = {
	.clks = venc_clks,
	.num_clks = ARRAY_SIZE(venc_clks),
};

static const struct of_device_id of_match_clk_mt6797_venc[] = {
	{
		.compatible = "mediatek,mt6797-vencsys",
		.data = &venc_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6797_venc);

static struct platform_driver clk_mt6797_venc_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6797-venc",
		.of_match_table = of_match_clk_mt6797_venc,
	},
};
module_platform_driver(clk_mt6797_venc_drv);
MODULE_LICENSE("GPL");
