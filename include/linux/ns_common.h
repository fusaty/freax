/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_NS_COMMON_H
#define _freax_NS_COMMON_H

#include <freax/refcount.h>

struct proc_ns_operations;

struct ns_common {
	atomic_long_t stashed;
	const struct proc_ns_operations *ops;
	unsigned int inum;
	refcount_t count;
};

#endif
