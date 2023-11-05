// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2017 Broadcom

#include <freax/kernel.h>
#include <freax/err.h>
#include <freax/clk-provider.h>
#include <freax/io.h>
#include <freax/of.h>
#include <freax/of_address.h>

#include "clk-iproc.h"

static void __init hr2_armpll_init(struct device_node *node)
{
	iproc_armpll_setup(node);
}
CLK_OF_DECLARE(hr2_armpll, "brcm,hr2-armpll", hr2_armpll_init);
