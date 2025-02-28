// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2014 STMicroelectronics (R&D) Limited
 */

/*
 * Authors:
 * Stephen Gallimore <stephen.gallimore@st.com>,
 * Pankaj Dev <pankaj.dev@st.com>.
 */

#include <freax/slab.h>
#include <freax/of_address.h>
#include <freax/clk.h>
#include <freax/clk-provider.h>
#include <freax/iopoll.h>

#include "clkgen.h"

static DEFINE_SPINLOCK(clkgena_c32_odf_lock);
DEFINE_SPINLOCK(clkgen_a9_lock);

/*
 * PLL configuration register bits for PLL3200 C32
 */
#define C32_NDIV_MASK (0xff)
#define C32_IDF_MASK (0x7)
#define C32_ODF_MASK (0x3f)
#define C32_LDF_MASK (0x7f)
#define C32_CP_MASK (0x1f)

#define C32_MAX_ODFS (4)

/*
 * PLL configuration register bits for PLL4600 C28
 */
#define C28_NDIV_MASK (0xff)
#define C28_IDF_MASK (0x7)
#define C28_ODF_MASK (0x3f)

struct clkgen_pll_data {
	struct clkgen_field pdn_status;
	struct clkgen_field pdn_ctrl;
	struct clkgen_field locked_status;
	struct clkgen_field mdiv;
	struct clkgen_field ndiv;
	struct clkgen_field pdiv;
	struct clkgen_field idf;
	struct clkgen_field ldf;
	struct clkgen_field cp;
	unsigned int num_odfs;
	struct clkgen_field odf[C32_MAX_ODFS];
	struct clkgen_field odf_gate[C32_MAX_ODFS];
	bool switch2pll_en;
	struct clkgen_field switch2pll;
	spinlock_t *lock;
	const struct clk_ops *ops;
};

struct clkgen_clk_out {
	const char *name;
	unsigned long flags;
};

struct clkgen_pll_data_clks {
	struct clkgen_pll_data *data;
	const struct clkgen_clk_out *outputs;
};


static const struct clk_ops stm_pll3200c32_ops;
static const struct clk_ops stm_pll3200c32_a9_ops;
static const struct clk_ops stm_pll4600c28_ops;

static const struct clkgen_pll_data st_pll3200c32_cx_0 = {
	/* 407 C0 PLL0 */
	.pdn_status	= CLKGEN_FIELD(0x2a0,	0x1,			8),
	.pdn_ctrl	= CLKGEN_FIELD(0x2a0,	0x1,			8),
	.locked_status	= CLKGEN_FIELD(0x2a0,	0x1,			24),
	.ndiv		= CLKGEN_FIELD(0x2a4,	C32_NDIV_MASK,		16),
	.idf		= CLKGEN_FIELD(0x2a4,	C32_IDF_MASK,		0x0),
	.num_odfs = 1,
	.odf		= { CLKGEN_FIELD(0x2b4, C32_ODF_MASK,		0) },
	.odf_gate	= { CLKGEN_FIELD(0x2b4, 0x1,			6) },
	.ops		= &stm_pll3200c32_ops,
};

static const struct clkgen_pll_data_clks st_pll3200c32_cx_0_legacy_data = {
	.data	= (struct clkgen_pll_data *)&st_pll3200c32_cx_0,
};

static const struct clkgen_clk_out st_pll3200c32_ax_0_clks[] = {
	{ .name = "clk-s-a0-pll-odf-0", },
};

static const struct clkgen_pll_data_clks st_pll3200c32_a0_data = {
	.data		= (struct clkgen_pll_data *)&st_pll3200c32_cx_0,
	.outputs	= st_pll3200c32_ax_0_clks,
};

static const struct clkgen_clk_out st_pll3200c32_cx_0_clks[] = {
	{ .name = "clk-s-c0-pll0-odf-0", },
};

static const struct clkgen_pll_data_clks st_pll3200c32_c0_data = {
	.data		= (struct clkgen_pll_data *)&st_pll3200c32_cx_0,
	.outputs	= st_pll3200c32_cx_0_clks,
};

static const struct clkgen_pll_data st_pll3200c32_cx_1 = {
	/* 407 C0 PLL1 */
	.pdn_status	= CLKGEN_FIELD(0x2c8,	0x1,			8),
	.pdn_ctrl	= CLKGEN_FIELD(0x2c8,	0x1,			8),
	.locked_status	= CLKGEN_FIELD(0x2c8,	0x1,			24),
	.ndiv		= CLKGEN_FIELD(0x2cc,	C32_NDIV_MASK,		16),
	.idf		= CLKGEN_FIELD(0x2cc,	C32_IDF_MASK,		0x0),
	.num_odfs = 1,
	.odf		= { CLKGEN_FIELD(0x2dc, C32_ODF_MASK,		0) },
	.odf_gate	= { CLKGEN_FIELD(0x2dc, 0x1,			6) },
	.ops		= &stm_pll3200c32_ops,
};

static const struct clkgen_pll_data_clks st_pll3200c32_cx_1_legacy_data = {
	.data	= (struct clkgen_pll_data *)&st_pll3200c32_cx_1,
};

static const struct clkgen_clk_out st_pll3200c32_cx_1_clks[] = {
	{ .name = "clk-s-c0-pll1-odf-0", },
};

static const struct clkgen_pll_data_clks st_pll3200c32_c1_data = {
	.data		= (struct clkgen_pll_data *)&st_pll3200c32_cx_1,
	.outputs	= st_pll3200c32_cx_1_clks,
};

static const struct clkgen_pll_data st_pll3200c32_407_a9 = {
	/* 407 A9 */
	.pdn_status	= CLKGEN_FIELD(0x1a8,	0x1,			0),
	.pdn_ctrl	= CLKGEN_FIELD(0x1a8,	0x1,			0),
	.locked_status	= CLKGEN_FIELD(0x87c,	0x1,			0),
	.ndiv		= CLKGEN_FIELD(0x1b0,	C32_NDIV_MASK,		0),
	.idf		= CLKGEN_FIELD(0x1a8,	C32_IDF_MASK,		25),
	.num_odfs = 1,
	.odf		= { CLKGEN_FIELD(0x1b0, C32_ODF_MASK,		8) },
	.odf_gate	= { CLKGEN_FIELD(0x1ac, 0x1,			28) },
	.switch2pll_en	= true,
	.cp		= CLKGEN_FIELD(0x1a8,	C32_CP_MASK,		1),
	.switch2pll	= CLKGEN_FIELD(0x1a4,	0x1,			1),
	.lock = &clkgen_a9_lock,
	.ops		= &stm_pll3200c32_a9_ops,
};

static const struct clkgen_clk_out st_pll3200c32_407_a9_clks[] = {
	{ .name = "clockgen-a9-pll-odf", },
};

static const struct clkgen_pll_data_clks st_pll3200c32_407_a9_data = {
	.data		= (struct clkgen_pll_data *)&st_pll3200c32_407_a9,
	.outputs	= st_pll3200c32_407_a9_clks,
};

static struct clkgen_pll_data st_pll4600c28_418_a9 = {
	/* 418 A9 */
	.pdn_status	= CLKGEN_FIELD(0x1a8,	0x1,			0),
	.pdn_ctrl	= CLKGEN_FIELD(0x1a8,	0x1,			0),
	.locked_status	= CLKGEN_FIELD(0x87c,	0x1,			0),
	.ndiv		= CLKGEN_FIELD(0x1b0,	C28_NDIV_MASK,		0),
	.idf		= CLKGEN_FIELD(0x1a8,	C28_IDF_MASK,		25),
	.num_odfs = 1,
	.odf		= { CLKGEN_FIELD(0x1b0, C28_ODF_MASK,		8) },
	.odf_gate	= { CLKGEN_FIELD(0x1ac, 0x1,			28) },
	.switch2pll_en	= true,
	.switch2pll	= CLKGEN_FIELD(0x1a4,	0x1,			1),
	.lock		= &clkgen_a9_lock,
	.ops		= &stm_pll4600c28_ops,
};

static const struct clkgen_clk_out st_pll4600c28_418_a9_clks[] = {
	{ .name = "clockgen-a9-pll-odf", },
};

static const struct clkgen_pll_data_clks st_pll4600c28_418_a9_data = {
	.data		= (struct clkgen_pll_data *)&st_pll4600c28_418_a9,
	.outputs	= st_pll4600c28_418_a9_clks,
};

/**
 * DOC: Clock Generated by PLL, rate set and enabled by bootloader
 *
 * Traits of this clock:
 * prepare - clk_(un)prepare only ensures parent is (un)prepared
 * enable - clk_enable/disable only ensures parent is enabled
 * rate - rate is fixed. No clk_set_rate support
 * parent - fixed parent.  No clk_set_parent support
 */

/*
 * PLL clock that is integrated in the ClockGenA instances on the STiH415
 * and STiH416.
 *
 * @hw: handle between common and hardware-specific interfaces.
 * @regs_base: base of the PLL configuration register(s).
 *
 */
struct clkgen_pll {
	struct clk_hw		hw;
	struct clkgen_pll_data	*data;
	void __iomem		*regs_base;
	spinlock_t	*lock;

	u32 ndiv;
	u32 idf;
	u32 cp;
};

#define to_clkgen_pll(_hw) container_of(_hw, struct clkgen_pll, hw)

struct stm_pll {
	unsigned long mdiv;
	unsigned long ndiv;
	unsigned long pdiv;
	unsigned long odf;
	unsigned long idf;
	unsigned long ldf;
	unsigned long cp;
};

static int clkgen_pll_is_locked(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	u32 locked = CLKGEN_READ(pll, locked_status);

	return !!locked;
}

static int clkgen_pll_is_enabled(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	u32 poweroff = CLKGEN_READ(pll, pdn_status);
	return !poweroff;
}

static int __clkgen_pll_enable(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	void __iomem *base =  pll->regs_base;
	struct clkgen_field *field = &pll->data->locked_status;
	int ret = 0;
	u32 reg;

	if (clkgen_pll_is_enabled(hw))
		return 0;

	CLKGEN_WRITE(pll, pdn_ctrl, 0);

	ret = readl_relaxed_poll_timeout(base + field->offset, reg,
			!!((reg >> field->shift) & field->mask),  0, 10000);

	if (!ret) {
		if (pll->data->switch2pll_en)
			CLKGEN_WRITE(pll, switch2pll, 0);

		pr_debug("%s:%s enabled\n", __clk_get_name(hw->clk), __func__);
	}

	return ret;
}

static int clkgen_pll_enable(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	unsigned long flags = 0;
	int ret = 0;

	if (pll->lock)
		spin_lock_irqsave(pll->lock, flags);

	ret = __clkgen_pll_enable(hw);

	if (pll->lock)
		spin_unlock_irqrestore(pll->lock, flags);

	return ret;
}

static void __clkgen_pll_disable(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);

	if (!clkgen_pll_is_enabled(hw))
		return;

	if (pll->data->switch2pll_en)
		CLKGEN_WRITE(pll, switch2pll, 1);

	CLKGEN_WRITE(pll, pdn_ctrl, 1);

	pr_debug("%s:%s disabled\n", __clk_get_name(hw->clk), __func__);
}

static void clkgen_pll_disable(struct clk_hw *hw)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	unsigned long flags = 0;

	if (pll->lock)
		spin_lock_irqsave(pll->lock, flags);

	__clkgen_pll_disable(hw);

	if (pll->lock)
		spin_unlock_irqrestore(pll->lock, flags);
}

static int clk_pll3200c32_get_params(unsigned long input, unsigned long output,
			  struct stm_pll *pll)
{
	unsigned long i, n;
	unsigned long deviation = ~0;
	unsigned long new_freq;
	long new_deviation;
	/* Charge pump table: highest ndiv value for cp=6 to 25 */
	static const unsigned char cp_table[] = {
		48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
		128, 136, 144, 152, 160, 168, 176, 184, 192
	};

	/* Output clock range: 800Mhz to 1600Mhz */
	if (output < 800000000 || output > 1600000000)
		return -EINVAL;

	input /= 1000;
	output /= 1000;

	for (i = 1; i <= 7 && deviation; i++) {
		n = i * output / (2 * input);

		/* Checks */
		if (n < 8)
			continue;
		if (n > 200)
			break;

		new_freq = (input * 2 * n) / i;

		new_deviation = abs(new_freq - output);

		if (!new_deviation || new_deviation < deviation) {
			pll->idf  = i;
			pll->ndiv = n;
			deviation = new_deviation;
		}
	}

	if (deviation == ~0) /* No solution found */
		return -EINVAL;

	/* Computing recommended charge pump value */
	for (pll->cp = 6; pll->ndiv > cp_table[pll->cp-6]; (pll->cp)++)
		;

	return 0;
}

static int clk_pll3200c32_get_rate(unsigned long input, struct stm_pll *pll,
			unsigned long *rate)
{
	if (!pll->idf)
		pll->idf = 1;

	*rate = ((2 * (input / 1000) * pll->ndiv) / pll->idf) * 1000;

	return 0;
}

static unsigned long recalc_stm_pll3200c32(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	unsigned long ndiv, idf;
	unsigned long rate = 0;

	if (!clkgen_pll_is_enabled(hw) || !clkgen_pll_is_locked(hw))
		return 0;

	ndiv = CLKGEN_READ(pll, ndiv);
	idf = CLKGEN_READ(pll, idf);

	if (idf)
		/* Note: input is divided to avoid overflow */
		rate = ((2 * (parent_rate/1000) * ndiv) / idf) * 1000;

	pr_debug("%s:%s rate %lu\n", clk_hw_get_name(hw), __func__, rate);

	return rate;
}

static long round_rate_stm_pll3200c32(struct clk_hw *hw, unsigned long rate,
		unsigned long *prate)
{
	struct stm_pll params;

	if (!clk_pll3200c32_get_params(*prate, rate, &params))
		clk_pll3200c32_get_rate(*prate, &params, &rate);
	else {
		pr_debug("%s: %s rate %ld Invalid\n", __func__,
			 __clk_get_name(hw->clk), rate);
		return 0;
	}

	pr_debug("%s: %s new rate %ld [ndiv=%u] [idf=%u]\n",
		 __func__, __clk_get_name(hw->clk),
		 rate, (unsigned int)params.ndiv,
		 (unsigned int)params.idf);

	return rate;
}

static int set_rate_stm_pll3200c32(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	struct stm_pll params;
	long hwrate = 0;
	unsigned long flags = 0;

	if (!rate || !parent_rate)
		return -EINVAL;

	if (!clk_pll3200c32_get_params(parent_rate, rate, &params))
		clk_pll3200c32_get_rate(parent_rate, &params, &hwrate);

	pr_debug("%s: %s new rate %ld [ndiv=0x%x] [idf=0x%x]\n",
		 __func__, __clk_get_name(hw->clk),
		 hwrate, (unsigned int)params.ndiv,
		 (unsigned int)params.idf);

	if (!hwrate)
		return -EINVAL;

	pll->ndiv = params.ndiv;
	pll->idf = params.idf;
	pll->cp = params.cp;

	__clkgen_pll_disable(hw);

	if (pll->lock)
		spin_lock_irqsave(pll->lock, flags);

	CLKGEN_WRITE(pll, ndiv, pll->ndiv);
	CLKGEN_WRITE(pll, idf, pll->idf);
	CLKGEN_WRITE(pll, cp, pll->cp);

	if (pll->lock)
		spin_unlock_irqrestore(pll->lock, flags);

	__clkgen_pll_enable(hw);

	return 0;
}

/* PLL output structure
 * FVCO >> /2 >> FVCOBY2 (no output)
 *                 |> Divider (ODF) >> PHI
 *
 * FVCOby2 output = (input * 2 * NDIV) / IDF (assuming FRAC_CONTROL==L)
 *
 * Rules:
 *   4Mhz <= INFF input <= 350Mhz
 *   4Mhz <= INFIN (INFF / IDF) <= 50Mhz
 *   19.05Mhz <= FVCOby2 output (PHI w ODF=1) <= 3000Mhz
 *   1 <= i (register/dec value for IDF) <= 7
 *   8 <= n (register/dec value for NDIV) <= 246
 */

static int clk_pll4600c28_get_params(unsigned long input, unsigned long output,
			  struct stm_pll *pll)
{

	unsigned long i, infin, n;
	unsigned long deviation = ~0;
	unsigned long new_freq, new_deviation;

	/* Output clock range: 19Mhz to 3000Mhz */
	if (output < 19000000 || output > 3000000000u)
		return -EINVAL;

	/* For better jitter, IDF should be smallest and NDIV must be maximum */
	for (i = 1; i <= 7 && deviation; i++) {
		/* INFIN checks */
		infin = input / i;
		if (infin < 4000000 || infin > 50000000)
			continue;	/* Invalid case */

		n = output / (infin * 2);
		if (n < 8 || n > 246)
			continue;	/* Invalid case */
		if (n < 246)
			n++;	/* To work around 'y' when n=x.y */

		for (; n >= 8 && deviation; n--) {
			new_freq = infin * 2 * n;
			if (new_freq < output)
				break;	/* Optimization: shorting loop */

			new_deviation = new_freq - output;
			if (!new_deviation || new_deviation < deviation) {
				pll->idf  = i;
				pll->ndiv = n;
				deviation = new_deviation;
			}
		}
	}

	if (deviation == ~0) /* No solution found */
		return -EINVAL;

	return 0;
}

static int clk_pll4600c28_get_rate(unsigned long input, struct stm_pll *pll,
			unsigned long *rate)
{
	if (!pll->idf)
		pll->idf = 1;

	*rate = (input / pll->idf) * 2 * pll->ndiv;

	return 0;
}

static unsigned long recalc_stm_pll4600c28(struct clk_hw *hw,
				    unsigned long parent_rate)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	struct stm_pll params;
	unsigned long rate;

	if (!clkgen_pll_is_enabled(hw) || !clkgen_pll_is_locked(hw))
		return 0;

	params.ndiv = CLKGEN_READ(pll, ndiv);
	params.idf = CLKGEN_READ(pll, idf);

	clk_pll4600c28_get_rate(parent_rate, &params, &rate);

	pr_debug("%s:%s rate %lu\n", __clk_get_name(hw->clk), __func__, rate);

	return rate;
}

static long round_rate_stm_pll4600c28(struct clk_hw *hw, unsigned long rate,
				      unsigned long *prate)
{
	struct stm_pll params;

	if (!clk_pll4600c28_get_params(*prate, rate, &params)) {
		clk_pll4600c28_get_rate(*prate, &params, &rate);
	} else {
		pr_debug("%s: %s rate %ld Invalid\n", __func__,
			 __clk_get_name(hw->clk), rate);
		return 0;
	}

	pr_debug("%s: %s new rate %ld [ndiv=%u] [idf=%u]\n",
		 __func__, __clk_get_name(hw->clk),
		 rate, (unsigned int)params.ndiv,
		 (unsigned int)params.idf);

	return rate;
}

static int set_rate_stm_pll4600c28(struct clk_hw *hw, unsigned long rate,
				   unsigned long parent_rate)
{
	struct clkgen_pll *pll = to_clkgen_pll(hw);
	struct stm_pll params;
	long hwrate;
	unsigned long flags = 0;

	if (!rate || !parent_rate)
		return -EINVAL;

	if (!clk_pll4600c28_get_params(parent_rate, rate, &params)) {
		clk_pll4600c28_get_rate(parent_rate, &params, &hwrate);
	} else {
		pr_debug("%s: %s rate %ld Invalid\n", __func__,
			 __clk_get_name(hw->clk), rate);
		return -EINVAL;
	}

	pr_debug("%s: %s new rate %ld [ndiv=0x%x] [idf=0x%x]\n",
		 __func__, __clk_get_name(hw->clk),
		 hwrate, (unsigned int)params.ndiv,
		 (unsigned int)params.idf);

	if (!hwrate)
		return -EINVAL;

	pll->ndiv = params.ndiv;
	pll->idf = params.idf;

	__clkgen_pll_disable(hw);

	if (pll->lock)
		spin_lock_irqsave(pll->lock, flags);

	CLKGEN_WRITE(pll, ndiv, pll->ndiv);
	CLKGEN_WRITE(pll, idf, pll->idf);

	if (pll->lock)
		spin_unlock_irqrestore(pll->lock, flags);

	__clkgen_pll_enable(hw);

	return 0;
}

static const struct clk_ops stm_pll3200c32_ops = {
	.enable		= clkgen_pll_enable,
	.disable	= clkgen_pll_disable,
	.is_enabled	= clkgen_pll_is_enabled,
	.recalc_rate	= recalc_stm_pll3200c32,
};

static const struct clk_ops stm_pll3200c32_a9_ops = {
	.enable		= clkgen_pll_enable,
	.disable	= clkgen_pll_disable,
	.is_enabled	= clkgen_pll_is_enabled,
	.recalc_rate	= recalc_stm_pll3200c32,
	.round_rate	= round_rate_stm_pll3200c32,
	.set_rate	= set_rate_stm_pll3200c32,
};

static const struct clk_ops stm_pll4600c28_ops = {
	.enable		= clkgen_pll_enable,
	.disable	= clkgen_pll_disable,
	.is_enabled	= clkgen_pll_is_enabled,
	.recalc_rate	= recalc_stm_pll4600c28,
	.round_rate	= round_rate_stm_pll4600c28,
	.set_rate	= set_rate_stm_pll4600c28,
};

static struct clk * __init clkgen_pll_register(const char *parent_name,
				struct clkgen_pll_data	*pll_data,
				void __iomem *reg, unsigned long pll_flags,
				const char *clk_name, spinlock_t *lock)
{
	struct clkgen_pll *pll;
	struct clk *clk;
	struct clk_init_data init;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	init.name = clk_name;
	init.ops = pll_data->ops;

	init.flags = pll_flags | CLK_GET_RATE_NOCACHE;
	init.parent_names = &parent_name;
	init.num_parents  = 1;

	pll->data = pll_data;
	pll->regs_base = reg;
	pll->hw.init = &init;
	pll->lock = lock;

	clk = clk_register(NULL, &pll->hw);
	if (IS_ERR(clk)) {
		kfree(pll);
		return clk;
	}

	pr_debug("%s: parent %s rate %lu\n",
			__clk_get_name(clk),
			__clk_get_name(clk_get_parent(clk)),
			clk_get_rate(clk));

	return clk;
}

static void __iomem * __init clkgen_get_register_base(
				struct device_node *np)
{
	struct device_node *pnode;
	void __iomem *reg = NULL;

	pnode = of_get_parent(np);
	if (!pnode)
		return NULL;

	reg = of_iomap(pnode, 0);

	of_node_put(pnode);
	return reg;
}

static struct clk * __init clkgen_odf_register(const char *parent_name,
					       void __iomem *reg,
					       struct clkgen_pll_data *pll_data,
					       unsigned long pll_flags, int odf,
					       spinlock_t *odf_lock,
					       const char *odf_name)
{
	struct clk *clk;
	unsigned long flags;
	struct clk_gate *gate;
	struct clk_divider *div;

	flags = pll_flags | CLK_GET_RATE_NOCACHE | CLK_SET_RATE_PARENT;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return ERR_PTR(-ENOMEM);

	gate->flags = CLK_GATE_SET_TO_DISABLE;
	gate->reg = reg + pll_data->odf_gate[odf].offset;
	gate->bit_idx = pll_data->odf_gate[odf].shift;
	gate->lock = odf_lock;

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div) {
		kfree(gate);
		return ERR_PTR(-ENOMEM);
	}

	div->flags = CLK_DIVIDER_ONE_BASED | CLK_DIVIDER_ALLOW_ZERO;
	div->reg = reg + pll_data->odf[odf].offset;
	div->shift = pll_data->odf[odf].shift;
	div->width = fls(pll_data->odf[odf].mask);
	div->lock = odf_lock;

	clk = clk_register_composite(NULL, odf_name, &parent_name, 1,
				     NULL, NULL,
				     &div->hw, &clk_divider_ops,
				     &gate->hw, &clk_gate_ops,
				     flags);
	if (IS_ERR(clk))
		return clk;

	pr_debug("%s: parent %s rate %lu\n",
			__clk_get_name(clk),
			__clk_get_name(clk_get_parent(clk)),
			clk_get_rate(clk));
	return clk;
}


static void __init clkgen_c32_pll_setup(struct device_node *np,
		struct clkgen_pll_data_clks *datac)
{
	struct clk *clk;
	const char *parent_name, *pll_name;
	void __iomem *pll_base;
	int num_odfs, odf;
	struct clk_onecell_data *clk_data;
	unsigned long pll_flags = 0;


	parent_name = of_clk_get_parent_name(np, 0);
	if (!parent_name)
		return;

	pll_base = clkgen_get_register_base(np);
	if (!pll_base)
		return;

	of_clk_detect_critical(np, 0, &pll_flags);

	clk = clkgen_pll_register(parent_name, datac->data, pll_base, pll_flags,
				  np->name, datac->data->lock);
	if (IS_ERR(clk))
		return;

	pll_name = __clk_get_name(clk);

	num_odfs = datac->data->num_odfs;

	clk_data = kzalloc(sizeof(*clk_data), GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->clk_num = num_odfs;
	clk_data->clks = kcalloc(clk_data->clk_num, sizeof(struct clk *),
				 GFP_KERNEL);

	if (!clk_data->clks)
		goto err;

	for (odf = 0; odf < num_odfs; odf++) {
		struct clk *clk;
		const char *clk_name;
		unsigned long odf_flags = 0;

		if (datac->outputs) {
			clk_name = datac->outputs[odf].name;
			odf_flags = datac->outputs[odf].flags;
		} else {
			if (of_property_read_string_index(np,
							  "clock-output-names",
							  odf, &clk_name))
				return;

			of_clk_detect_critical(np, odf, &odf_flags);
		}

		clk = clkgen_odf_register(pll_name, pll_base, datac->data,
				odf_flags, odf, &clkgena_c32_odf_lock,
				clk_name);
		if (IS_ERR(clk))
			goto err;

		clk_data->clks[odf] = clk;
	}

	of_clk_add_provider(np, of_clk_src_onecell_get, clk_data);
	return;

err:
	kfree(pll_name);
	kfree(clk_data->clks);
	kfree(clk_data);
}
static void __init clkgen_c32_pll0_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_cx_0_legacy_data);
}
CLK_OF_DECLARE(c32_pll0, "st,clkgen-pll0", clkgen_c32_pll0_setup);

static void __init clkgen_c32_pll0_a0_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_a0_data);
}
CLK_OF_DECLARE(c32_pll0_a0, "st,clkgen-pll0-a0", clkgen_c32_pll0_a0_setup);

static void __init clkgen_c32_pll0_c0_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_c0_data);
}
CLK_OF_DECLARE(c32_pll0_c0, "st,clkgen-pll0-c0", clkgen_c32_pll0_c0_setup);

static void __init clkgen_c32_pll1_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_cx_1_legacy_data);
}
CLK_OF_DECLARE(c32_pll1, "st,clkgen-pll1", clkgen_c32_pll1_setup);

static void __init clkgen_c32_pll1_c0_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_c1_data);
}
CLK_OF_DECLARE(c32_pll1_c0, "st,clkgen-pll1-c0", clkgen_c32_pll1_c0_setup);

static void __init clkgen_c32_plla9_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll3200c32_407_a9_data);
}
CLK_OF_DECLARE(c32_plla9, "st,stih407-clkgen-plla9", clkgen_c32_plla9_setup);

static void __init clkgen_c28_plla9_setup(struct device_node *np)
{
	clkgen_c32_pll_setup(np,
		(struct clkgen_pll_data_clks *) &st_pll4600c28_418_a9_data);
}
CLK_OF_DECLARE(c28_plla9, "st,stih418-clkgen-plla9", clkgen_c28_plla9_setup);
