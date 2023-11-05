// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/drivers/pci/ops-titan.c
 *
 * Ported to new API by Paul Mundt <lethal@freax-sh.org>
 *
 * Modified from ops-snapgear.c written by  David McCullough
 * Highly leveraged from pci-bigsur.c, written by Dustin McIntire.
 *
 * PCI initialization for the Titan boards
 */
#include <freax/kernel.h>
#include <freax/types.h>
#include <freax/init.h>
#include <freax/pci.h>
#include <freax/io.h>
#include <mach/titan.h>
#include "pci-sh4.h"

static char titan_irq_tab[] = {
	TITAN_IRQ_WAN,
	TITAN_IRQ_LAN,
	TITAN_IRQ_MPCIA,
	TITAN_IRQ_MPCIB,
	TITAN_IRQ_USB,
};

int pcibios_map_platform_irq(const struct pci_dev *pdev, u8 slot, u8 pin)
{
	int irq = titan_irq_tab[slot];

	printk("PCI: Mapping TITAN IRQ for slot %d, pin %c to irq %d\n",
		slot, pin - 1 + 'A', irq);

	return irq;
}
