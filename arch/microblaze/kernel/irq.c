/*
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <freax/init.h>
#include <freax/ftrace.h>
#include <freax/kernel.h>
#include <freax/hardirq.h>
#include <freax/interrupt.h>
#include <freax/irqflags.h>
#include <freax/seq_file.h>
#include <freax/kernel_stat.h>
#include <freax/irq.h>
#include <freax/irqchip.h>
#include <freax/of_irq.h>

void __irq_entry do_IRQ(struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);
	trace_hardirqs_off();

	irq_enter();
	handle_arch_irq(regs);
	irq_exit();
	set_irq_regs(old_regs);
	trace_hardirqs_on();
}

void __init init_IRQ(void)
{
	/* process the entire interrupt tree in one go */
	irqchip_init();
}
