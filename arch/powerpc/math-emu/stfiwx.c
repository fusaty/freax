// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>
#include <freax/errno.h>
#include <freax/uaccess.h>

int
stfiwx(u32 *frS, void *ea)
{
#ifdef DEBUG
	printk("%s: %p %p\n", __func__, frS, ea);
#endif

	if (copy_to_user(ea, &frS[1], sizeof(frS[1])))
		return -EFAULT;

	return 0;
}
