/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _M68K_FCNTL_H
#define _M68K_FCNTL_H

#define O_DIRECTORY	040000	/* must be a directory */
#define O_NOFOLLOW	0100000	/* don't follow links */
#define O_DIRECT	0200000	/* direct disk access hint - currently ignored */
#define O_LARGEFILE	0400000

#include <asm-generic/fcntl.h>

#endif /* _M68K_FCNTL_H */
