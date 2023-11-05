// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/errno.h>
#include <freax/uaccess.h>

#include <asm/sfp-machine.h>
#include <math-emu/soft-fp.h>

int
mtfsb0(int crbD)
{
	if ((crbD != 1) && (crbD != 2))
		__FPU_FPSCR &= ~(1 << (31 - crbD));

#ifdef DEBUG
	printk("%s: %d %08lx\n", __func__, crbD, __FPU_FPSCR);
#endif

	return 0;
}
