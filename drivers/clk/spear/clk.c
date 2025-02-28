// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 ST Microelectronics
 * Viresh Kumar <vireshk@kernel.org>
 *
 * SPEAr clk - Common routines
 */

#include <freax/clk-provider.h>
#include <freax/types.h>
#include "clk.h"

long clk_round_rate_index(struct clk_hw *hw, unsigned long drate,
		unsigned long parent_rate, clk_calc_rate calc_rate, u8 rtbl_cnt,
		int *index)
{
	unsigned long prev_rate, rate = 0;

	for (*index = 0; *index < rtbl_cnt; (*index)++) {
		prev_rate = rate;
		rate = calc_rate(hw, parent_rate, *index);
		if (drate < rate) {
			/* previous clock was best */
			if (*index) {
				rate = prev_rate;
				(*index)--;
			}
			break;
		}
	}

	if ((*index) == rtbl_cnt)
		(*index)--;

	return rate;
}
