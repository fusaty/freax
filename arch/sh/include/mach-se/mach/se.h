/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SH_HITACHI_SE_H
#define __ASM_SH_HITACHI_SE_H

/*
 * freax/include/asm-sh/hitachi_se.h
 *
 * Copyright (C) 2000  Kazumoto Kojima
 *
 * Hitachi SolutionEngine support
 */
#include <freax/sh_intc.h>

/* Box specific addresses.  */

#define PA_ROM		0x00000000	/* EPROM */
#define PA_ROM_SIZE	0x00400000	/* EPROM size 4M byte */
#define PA_FROM		0x01000000	/* EPROM */
#define PA_FROM_SIZE	0x00400000	/* EPROM size 4M byte */
#define PA_EXT1		0x04000000
#define PA_EXT1_SIZE	0x04000000
#define PA_EXT2		0x08000000
#define PA_EXT2_SIZE	0x04000000
#define PA_SDRAM	0x0c000000
#define PA_SDRAM_SIZE	0x04000000

#define PA_EXT4		0x12000000
#define PA_EXT4_SIZE	0x02000000
#define PA_EXT5		0x14000000
#define PA_EXT5_SIZE	0x04000000
#define PA_PCIC		0x18000000	/* MR-SHPC-01 PCMCIA */

#define PA_83902	0xb0000000	/* DP83902A */
#define PA_83902_IF	0xb0040000	/* DP83902A remote io port */
#define PA_83902_RST	0xb0080000	/* DP83902A reset port */

#define PA_SUPERIO	0xb0400000	/* SMC37C935A super io chip */
#define PA_DIPSW0	0xb0800000	/* Dip switch 5,6 */
#define PA_DIPSW1	0xb0800002	/* Dip switch 7,8 */
#define PA_LED		0xb0c00000	/* LED */
#if defined(CONFIG_CPU_SUBTYPE_SH7705)
#define PA_BCR		0xb0e00000
#else
#define PA_BCR		0xb1400000	/* FPGA */
#endif

#define PA_MRSHPC	0xb83fffe0	/* MR-SHPC-01 PCMCIA controller */
#define PA_MRSHPC_MW1	0xb8400000	/* MR-SHPC-01 memory window base */
#define PA_MRSHPC_MW2	0xb8500000	/* MR-SHPC-01 attribute window base */
#define PA_MRSHPC_IO	0xb8600000	/* MR-SHPC-01 I/O window base */
#define MRSHPC_OPTION   (PA_MRSHPC + 6)
#define MRSHPC_CSR      (PA_MRSHPC + 8)
#define MRSHPC_ISR      (PA_MRSHPC + 10)
#define MRSHPC_ICR      (PA_MRSHPC + 12)
#define MRSHPC_CPWCR    (PA_MRSHPC + 14)
#define MRSHPC_MW0CR1   (PA_MRSHPC + 16)
#define MRSHPC_MW1CR1   (PA_MRSHPC + 18)
#define MRSHPC_IOWCR1   (PA_MRSHPC + 20)
#define MRSHPC_MW0CR2   (PA_MRSHPC + 22)
#define MRSHPC_MW1CR2   (PA_MRSHPC + 24)
#define MRSHPC_IOWCR2   (PA_MRSHPC + 26)
#define MRSHPC_CDCR     (PA_MRSHPC + 28)
#define MRSHPC_PCIC_INFO (PA_MRSHPC + 30)

#define BCR_ILCRA	(PA_BCR + 0)
#define BCR_ILCRB	(PA_BCR + 2)
#define BCR_ILCRC	(PA_BCR + 4)
#define BCR_ILCRD	(PA_BCR + 6)
#define BCR_ILCRE	(PA_BCR + 8)
#define BCR_ILCRF	(PA_BCR + 10)
#define BCR_ILCRG	(PA_BCR + 12)

#if defined(CONFIG_CPU_SUBTYPE_SH7709)
#define INTC_IRR0       0xa4000004UL
#define INTC_IRR1       0xa4000006UL
#define INTC_IRR2       0xa4000008UL

#define INTC_ICR0       0xfffffee0UL
#define INTC_ICR1       0xa4000010UL
#define INTC_ICR2       0xa4000012UL
#define INTC_INTER      0xa4000014UL

#define INTC_IPRC       0xa4000016UL
#define INTC_IPRD       0xa4000018UL
#define INTC_IPRE       0xa400001aUL

#define IRQ0_IRQ        evt2irq(0x600)
#define IRQ1_IRQ        evt2irq(0x620)
#endif

#if defined(CONFIG_CPU_SUBTYPE_SH7705)
#define IRQ_STNIC	evt2irq(0x380)
#define IRQ_CFCARD	evt2irq(0x3c0)
#else
#define IRQ_STNIC	evt2irq(0x340)
#define IRQ_CFCARD	evt2irq(0x2e0)
#endif

/* SH Ether support (SH7710/SH7712) */
/* Base address */
#define SH_ETH0_BASE 0xA7000000
#define SH_ETH1_BASE 0xA7000400
#define SH_TSU_BASE  0xA7000800
/* PHY ID */
#if defined(CONFIG_CPU_SUBTYPE_SH7710)
# define PHY_ID 0x00
#elif defined(CONFIG_CPU_SUBTYPE_SH7712)
# define PHY_ID 0x01
#endif
/* Ether IRQ */
#define SH_ETH0_IRQ	evt2irq(0xc00)
#define SH_ETH1_IRQ	evt2irq(0xc20)
#define SH_TSU_IRQ	evt2irq(0xc40)

void init_se_IRQ(void);

#define __IO_PREFIX	se
#include <asm/io_generic.h>

#endif  /* __ASM_SH_HITACHI_SE_H */
