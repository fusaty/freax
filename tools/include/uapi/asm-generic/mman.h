/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __ASM_GENERIC_MMAN_H
#define __ASM_GENERIC_MMAN_H

#include <asm-generic/mman-common-tools.h>

#define MAP_GROWSDOWN	0x0100		/* stack-like segment */
#define MAP_DENYWRITE	0x0800		/* ETXTBSY */
#define MAP_EXECUTABLE	0x1000		/* mark it as an executable */
#define MAP_LOCKED	0x2000		/* pages are locked */
#define MAP_NORESERVE	0x4000		/* don't check for reservations */

/*
 * Bits [26:31] are reserved, see asm-generic/hugetlb_encode.h
 * for MAP_HUGETLB usage
 */

#define MCL_CURRENT	1		/* lock all current mappings */
#define MCL_FUTURE	2		/* lock all future mappings */
#define MCL_ONFAULT	4		/* lock all pages that are faulted in */

#endif /* __ASM_GENERIC_MMAN_H */
