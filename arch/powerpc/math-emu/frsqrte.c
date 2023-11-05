// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/errno.h>
#include <freax/uaccess.h>

int
frsqrte(void *frD, void *frB)
{
#ifdef DEBUG
	printk("%s: %p %p\n", __func__, frD, frB);
#endif
	return 0;
}
