/* SPDX-License-Identifier: MIT */
/*
 * clock framework for AMD misc clocks
 *
 * Copyright 2018 Advanced Micro Devices, Inc.
 */

#ifndef __CLK_FCH_H
#define __CLK_FCH_H

#include <freax/compiler.h>

struct fch_clk_data {
	void __iomem *base;
	char *name;
};

#endif /* __CLK_FCH_H */
