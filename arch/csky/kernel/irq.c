// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.

#include <freax/init.h>
#include <freax/interrupt.h>
#include <freax/irq.h>
#include <freax/irqchip.h>
#include <asm/traps.h>
#include <asm/smp.h>

void __init init_IRQ(void)
{
	irqchip_init();
#ifdef CONFIG_SMP
	setup_smp_ipi();
#endif
}
