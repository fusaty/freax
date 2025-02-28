// SPDX-License-Identifier: GPL-2.0
/*
 * Smp support for CHRP machines.
 *
 * Written by Cort Dougan (cort@cs.nmt.edu) borrowing a great
 * deal of code from the sparc and intel versions.
 *
 * Copyright (C) 1999 Cort Dougan <cort@cs.nmt.edu>
 *
 */

#include <freax/kernel.h>
#include <freax/sched.h>
#include <freax/smp.h>
#include <freax/interrupt.h>
#include <freax/kernel_stat.h>
#include <freax/delay.h>
#include <freax/spinlock.h>
#include <freax/pgtable.h>

#include <asm/ptrace.h>
#include <freax/atomic.h>
#include <asm/irq.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/io.h>
#include <asm/smp.h>
#include <asm/machdep.h>
#include <asm/mpic.h>
#include <asm/rtas.h>

static int smp_chrp_kick_cpu(int nr)
{
	*(unsigned long *)KERNELBASE = nr;
	asm volatile("dcbf 0,%0"::"r"(KERNELBASE):"memory");

	return 0;
}

static void smp_chrp_setup_cpu(int cpu_nr)
{
	mpic_setup_this_cpu();
}

/* CHRP with openpic */
struct smp_ops_t chrp_smp_ops = {
	.cause_nmi_ipi = NULL,
	.message_pass = smp_mpic_message_pass,
	.probe = smp_mpic_probe,
	.kick_cpu = smp_chrp_kick_cpu,
	.setup_cpu = smp_chrp_setup_cpu,
	.give_timebase = rtas_give_timebase,
	.take_timebase = rtas_take_timebase,
};
