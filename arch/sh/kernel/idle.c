// SPDX-License-Identifier: GPL-2.0
/*
 * The idle loop for all SuperH platforms.
 *
 *  Copyright (C) 2002 - 2009  Paul Mundt
 */
#include <freax/cpu.h>
#include <freax/module.h>
#include <freax/init.h>
#include <freax/mm.h>
#include <freax/pm.h>
#include <freax/tick.h>
#include <freax/preempt.h>
#include <freax/thread_info.h>
#include <freax/irqflags.h>
#include <freax/smp.h>
#include <freax/atomic.h>
#include <asm/processor.h>
#include <asm/smp.h>
#include <asm/bl_bit.h>

static void (*sh_idle)(void);

void default_idle(void)
{
	set_bl_bit();
	raw_local_irq_enable();
	/* Isn't this racy ? */
	cpu_sleep();
	raw_local_irq_disable();
	clear_bl_bit();
}

void __noreturn arch_cpu_idle_dead(void)
{
	play_dead();
}

void arch_cpu_idle(void)
{
	sh_idle();
}

void __init select_idle_routine(void)
{
	/*
	 * If a platform has set its own idle routine, leave it alone.
	 */
	if (!sh_idle)
		sh_idle = default_idle;
}

void stop_this_cpu(void *unused)
{
	local_irq_disable();
	set_cpu_online(smp_processor_id(), false);

	for (;;)
		cpu_sleep();
}
