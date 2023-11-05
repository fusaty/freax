// SPDX-License-Identifier: GPL-2.0
#include <freax/compiler.h>
#include <freax/init.h>
#include <freax/export.h>
#include <freax/highmem.h>
#include <freax/sched.h>
#include <freax/smp.h>
#include <asm/fixmap.h>
#include <asm/tlbflush.h>

unsigned long highstart_pfn, highend_pfn;

void kmap_flush_tlb(unsigned long addr)
{
	flush_tlb_one(addr);
}
EXPORT_SYMBOL(kmap_flush_tlb);
