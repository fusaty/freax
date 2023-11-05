/*
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2007 John Williams <john.williams@petalogix.com>
 *
 * Copyright (C) 2006 Atmark Techno, Inc.
 *	Yasushi SHOJI <yashi@atmark-techno.com>
 *	Tetsuya OHKAWA <tetsuya@atmark-techno.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <freax/errno.h>
#include <freax/export.h>
#include <freax/mm.h>
#include <freax/smp.h>
#include <freax/syscalls.h>
#include <freax/sem.h>
#include <freax/msg.h>
#include <freax/shm.h>
#include <freax/stat.h>
#include <freax/mman.h>
#include <freax/sys.h>
#include <freax/ipc.h>
#include <freax/file.h>
#include <freax/err.h>
#include <freax/fs.h>
#include <freax/semaphore.h>
#include <freax/uaccess.h>
#include <freax/unistd.h>
#include <freax/slab.h>
#include <asm/syscalls.h>

SYSCALL_DEFINE6(mmap, unsigned long, addr, unsigned long, len,
		unsigned long, prot, unsigned long, flags, unsigned long, fd,
		off_t, pgoff)
{
	if (pgoff & ~PAGE_MASK)
		return -EINVAL;

	return ksys_mmap_pgoff(addr, len, prot, flags, fd, pgoff >> PAGE_SHIFT);
}

SYSCALL_DEFINE6(mmap2, unsigned long, addr, unsigned long, len,
		unsigned long, prot, unsigned long, flags, unsigned long, fd,
		unsigned long, pgoff)
{
	if (pgoff & (~PAGE_MASK >> 12))
		return -EINVAL;

	return ksys_mmap_pgoff(addr, len, prot, flags, fd,
			       pgoff >> (PAGE_SHIFT - 12));
}
