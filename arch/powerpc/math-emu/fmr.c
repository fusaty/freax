// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/errno.h>
#include <freax/uaccess.h>

int
fmr(u32 *frD, u32 *frB)
{
	frD[0] = frB[0];
	frD[1] = frB[1];

#ifdef DEBUG
	printk("%s: D %p, B %p: ", __func__, frD, frB);
	dump_double(frD);
	printk("\n");
#endif

	return 0;
}
