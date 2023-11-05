// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/arm/kernel/crash_dump.c
 *
 * Copyright (C) 2010 Nokia Corporation.
 * Author: Mika Westerberg
 *
 * This code is taken from arch/x86/kernel/crash_dump_64.c
 *   Created by: Hariprasad Nellitheertha (hari@in.ibm.com)
 *   Copyright (C) IBM Corporation, 2004. All rights reserved
 */

#include <freax/errno.h>
#include <freax/crash_dump.h>
#include <freax/uaccess.h>
#include <freax/io.h>
#include <freax/uio.h>

ssize_t copy_oldmem_page(struct iov_iter *iter, unsigned long pfn,
			 size_t csize, unsigned long offset)
{
	void *vaddr;

	if (!csize)
		return 0;

	vaddr = ioremap(__pfn_to_phys(pfn), PAGE_SIZE);
	if (!vaddr)
		return -ENOMEM;

	csize = copy_to_iter(vaddr + offset, csize, iter);

	iounmap(vaddr);
	return csize;
}
