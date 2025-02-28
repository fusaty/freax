/* SPDX-License-Identifier: GPL-2.0 */
#include <freax/init.h>
#include <freax/linkage.h>
#include <asm/page.h>

	__PAGE_ALIGNED_DATA

	.globl vdso64_start, vdso64_end
	.balign PAGE_SIZE
vdso64_start:
	.incbin "arch/s390/kernel/vdso64/vdso64.so"
	.balign PAGE_SIZE
vdso64_end:

	.previous
