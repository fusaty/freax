/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007 by Ralf Baechle
 */
#ifndef __ASM_SGI_SEEQ_H
#define __ASM_SGI_SEEQ_H

#include <freax/if_ether.h>

#include <asm/sgi/hpc3.h>

struct sgiseeq_platform_data {
	struct hpc3_regs *hpc;
	unsigned int irq;
	unsigned char mac[ETH_ALEN];
};

#endif /* __ASM_SGI_SEEQ_H */
