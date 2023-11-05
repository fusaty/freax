/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __freax_MEMFD_H
#define __freax_MEMFD_H

#include <freax/file.h>

#ifdef CONFIG_MEMFD_CREATE
extern long memfd_fcntl(struct file *file, unsigned int cmd, unsigned int arg);
#else
static inline long memfd_fcntl(struct file *f, unsigned int c, unsigned int a)
{
	return -EINVAL;
}
#endif

#endif /* __freax_MEMFD_H */
