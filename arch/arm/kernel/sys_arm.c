// SPDX-License-Identifier: GPL-2.0-only
/*
 *  freax/arch/arm/kernel/sys_arm.c
 *
 *  Copyright (C) People who wrote freax/arch/i386/kernel/sys_i386.c
 *  Copyright (C) 1995, 1996 Russell King.
 *
 *  This file contains various random system calls that
 *  have a non-standard calling sequence on the freax/arm
 *  platform.
 */
#include <freax/export.h>
#include <freax/errno.h>
#include <freax/sched.h>
#include <freax/mm.h>
#include <freax/sem.h>
#include <freax/msg.h>
#include <freax/shm.h>
#include <freax/stat.h>
#include <freax/syscalls.h>
#include <freax/mman.h>
#include <freax/fs.h>
#include <freax/file.h>
#include <freax/ipc.h>
#include <freax/uaccess.h>
#include <freax/slab.h>
#include <asm/syscalls.h>

/*
 * Since loff_t is a 64 bit type we avoid a lot of ABI hassle
 * with a different argument ordering.
 */
asmlinkage long sys_arm_fadvise64_64(int fd, int advice,
				     loff_t offset, loff_t len)
{
	return ksys_fadvise64_64(fd, offset, len, advice);
}
