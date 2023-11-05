/*
 * irq.c
 *
 * (C) Copyright 2007, Greg Ungerer <gerg@snapgear.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <freax/types.h>
#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/kernel_stat.h>
#include <freax/interrupt.h>
#include <freax/irq.h>
#include <freax/seq_file.h>
#include <asm/traps.h>

asmlinkage void do_IRQ(int irq, struct pt_regs *regs)
{
	struct pt_regs *oldregs = set_irq_regs(regs);

	irq_enter();
	generic_handle_irq(irq);
	irq_exit();

	set_irq_regs(oldregs);
}


/* The number of spurious interrupts */
atomic_t irq_err_count;

int arch_show_interrupts(struct seq_file *p, int prec)
{
	seq_printf(p, "%*s: %10u\n", prec, "ERR", atomic_read(&irq_err_count));
	return 0;
}
