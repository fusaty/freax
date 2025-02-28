/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  freax/arch/arm/mm/tlbv4wbi.S
 *
 *  Copyright (C) 1997-2002 Russell King
 *
 *  ARM architecture version 4 and version 5 TLB handling functions.
 *  These assume a split I/D TLBs, with a write buffer.
 *
 *  Processors: ARM920 ARM922 ARM925 ARM926 XScale
 */
#include <freax/linkage.h>
#include <freax/init.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <asm/tlbflush.h>
#include "proc-macros.S"

/*
 *	v4wb_flush_user_tlb_range(start, end, mm)
 *
 *	Invalidate a range of TLB entries in the specified address space.
 *
 *	- start - range start address
 *	- end   - range end address
 *	- mm    - mm_struct describing address space
 */
	.align	5
ENTRY(v4wbi_flush_user_tlb_range)
	vma_vm_mm ip, r2
	act_mm	r3				@ get current->active_mm
	eors	r3, ip, r3			@ == mm ?
	retne	lr				@ no, we dont do anything
	mov	r3, #0
	mcr	p15, 0, r3, c7, c10, 4		@ drain WB
	vma_vm_flags r2, r2
	bic	r0, r0, #0x0ff
	bic	r0, r0, #0xf00
1:	tst	r2, #VM_EXEC
	mcrne	p15, 0, r0, c8, c5, 1		@ invalidate I TLB entry
	mcr	p15, 0, r0, c8, c6, 1		@ invalidate D TLB entry
	add	r0, r0, #PAGE_SZ
	cmp	r0, r1
	blo	1b
	ret	lr

ENTRY(v4wbi_flush_kern_tlb_range)
	mov	r3, #0
	mcr	p15, 0, r3, c7, c10, 4		@ drain WB
	bic	r0, r0, #0x0ff
	bic	r0, r0, #0xf00
1:	mcr	p15, 0, r0, c8, c5, 1		@ invalidate I TLB entry
	mcr	p15, 0, r0, c8, c6, 1		@ invalidate D TLB entry
	add	r0, r0, #PAGE_SZ
	cmp	r0, r1
	blo	1b
	ret	lr

	__INITDATA

	/* define struct cpu_tlb_fns (see <asm/tlbflush.h> and proc-macros.S) */
	define_tlb_functions v4wbi, v4wbi_tlb_flags
