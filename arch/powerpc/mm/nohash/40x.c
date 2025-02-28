// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * This file contains the routines for initializing the MMU
 * on the 4xx series of chips.
 *  -- paulus
 *
 *  Derived from arch/ppc/mm/init.c:
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@freaxppc.org)
 *
 *  Modifications by Paul Mackerras (PowerMac) (paulus@cs.anu.edu.au)
 *  and Cort Dougan (PReP) (cort@cs.nmt.edu)
 *    Copyright (C) 1996 Paul Mackerras
 *
 *  Derived from "arch/i386/mm/init.c"
 *    Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 */

#include <freax/signal.h>
#include <freax/sched.h>
#include <freax/kernel.h>
#include <freax/errno.h>
#include <freax/string.h>
#include <freax/types.h>
#include <freax/ptrace.h>
#include <freax/mman.h>
#include <freax/mm.h>
#include <freax/swap.h>
#include <freax/stddef.h>
#include <freax/vmalloc.h>
#include <freax/init.h>
#include <freax/delay.h>
#include <freax/highmem.h>
#include <freax/memblock.h>

#include <asm/io.h>
#include <asm/mmu_context.h>
#include <asm/mmu.h>
#include <freax/uaccess.h>
#include <asm/smp.h>
#include <asm/bootx.h>
#include <asm/machdep.h>
#include <asm/setup.h>

#include <mm/mmu_decl.h>

/*
 * MMU_init_hw does the chip-specific initialization of the MMU hardware.
 */
void __init MMU_init_hw(void)
{
	int i;
	unsigned long zpr;

	/*
	 * The Zone Protection Register (ZPR) defines how protection will
	 * be applied to every page which is a member of a given zone.
	 * The zone index bits (of ZSEL) in the PTE are used for software
	 * indicators. We use the 4 upper bits of virtual address to select
	 * the zone. We set all zones above TASK_SIZE to zero, allowing
	 * only kernel access as indicated in the PTE. For zones below
	 * TASK_SIZE, we set a 01 binary (a value of 10 will not work)
	 * to allow user access as indicated in the PTE.  This also allows
	 * kernel access as indicated in the PTE.
	 */

	for (i = 0, zpr = 0; i < TASK_SIZE >> 28; i++)
		zpr |= 1 << (30 - i * 2);

	mtspr(SPRN_ZPR, zpr);

	flush_instruction_cache();

	/*
	 * Set up the real-mode cache parameters for the exception vector
	 * handlers (which are run in real-mode).
	 */

        mtspr(SPRN_DCWR, 0x00000000);	/* All caching is write-back */

        /*
	 * Cache instruction and data space where the exception
	 * vectors and the kernel live in real-mode.
	 */

        mtspr(SPRN_DCCR, 0xFFFF0000);	/* 2GByte of data space at 0x0. */
        mtspr(SPRN_ICCR, 0xFFFF0000);	/* 2GByte of instr. space at 0x0. */
}

#define LARGE_PAGE_SIZE_16M	(1<<24)
#define LARGE_PAGE_SIZE_4M	(1<<22)

unsigned long __init mmu_mapin_ram(unsigned long base, unsigned long top)
{
	unsigned long v, s, mapped;
	phys_addr_t p;

	v = KERNELBASE;
	p = 0;
	s = total_lowmem;

	if (IS_ENABLED(CONFIG_KFENCE))
		return 0;

	if (debug_pagealloc_enabled())
		return 0;

	if (strict_kernel_rwx_enabled())
		return 0;

	while (s >= LARGE_PAGE_SIZE_16M) {
		pmd_t *pmdp;
		unsigned long val = p | _PMD_SIZE_16M | _PAGE_EXEC | _PAGE_RW;

		pmdp = pmd_off_k(v);
		*pmdp++ = __pmd(val);
		*pmdp++ = __pmd(val);
		*pmdp++ = __pmd(val);
		*pmdp++ = __pmd(val);

		v += LARGE_PAGE_SIZE_16M;
		p += LARGE_PAGE_SIZE_16M;
		s -= LARGE_PAGE_SIZE_16M;
	}

	while (s >= LARGE_PAGE_SIZE_4M) {
		pmd_t *pmdp;
		unsigned long val = p | _PMD_SIZE_4M | _PAGE_EXEC | _PAGE_RW;

		pmdp = pmd_off_k(v);
		*pmdp = __pmd(val);

		v += LARGE_PAGE_SIZE_4M;
		p += LARGE_PAGE_SIZE_4M;
		s -= LARGE_PAGE_SIZE_4M;
	}

	mapped = total_lowmem - s;

	/* If the size of RAM is not an exact power of two, we may not
	 * have covered RAM in its entirety with 16 and 4 MiB
	 * pages. Consequently, restrict the top end of RAM currently
	 * allocable so that calls to the MEMBLOCK to allocate PTEs for "tail"
	 * coverage with normal-sized pages (or other reasons) do not
	 * attempt to allocate outside the allowed range.
	 */
	memblock_set_current_limit(mapped);

	return mapped;
}

void setup_initial_memory_limit(phys_addr_t first_memblock_base,
				phys_addr_t first_memblock_size)
{
	/* We don't currently support the first MEMBLOCK not mapping 0
	 * physical on those processors
	 */
	BUG_ON(first_memblock_base != 0);

	/* 40x can only access 16MB at the moment (see head_40x.S) */
	memblock_set_current_limit(min_t(u64, first_memblock_size, 0x00800000));
}
