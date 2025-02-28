/* SPDX-License-Identifier: GPL-2.0-or-later */
/*  Architecture specific parts of HP's STI (framebuffer) driver.
 *  Structures are HP-UX compatible for XFree86 usage.
 * 
 *    freax/PA-RISC Project (http://www.parisc-freax.org/)
 *    Copyright (C) 2001 Helge Deller (deller a parisc-freax org)
 */

#ifndef __ASM_PARISC_GRFIOCTL_H
#define __ASM_PARISC_GRFIOCTL_H

/* upper 32 bits of graphics id (HP/UX identifier) */

#define GRFGATOR		8
#define S9000_ID_S300		9
#define GRFBOBCAT		9
#define	GRFCATSEYE		9
#define S9000_ID_98720		10
#define GRFRBOX			10
#define S9000_ID_98550		11
#define GRFFIREEYE		11
#define S9000_ID_A1096A		12
#define GRFHYPERION		12
#define S9000_ID_FRI		13
#define S9000_ID_98730		14
#define GRFDAVINCI		14
#define S9000_ID_98705		0x26C08070	/* Tigershark */
#define S9000_ID_98736		0x26D148AB
#define S9000_ID_A1659A		0x26D1482A	/* CRX 8 plane color (=ELK) */
#define S9000_ID_ELK		S9000_ID_A1659A
#define S9000_ID_A1439A		0x26D148EE	/* CRX24 = CRX+ (24-plane color) */
#define S9000_ID_A1924A		0x26D1488C	/* GRX gray-scale */
#define S9000_ID_ELM		S9000_ID_A1924A
#define S9000_ID_98765		0x27480DEF
#define S9000_ID_ELK_768	0x27482101
#define S9000_ID_STINGER	0x27A4A402
#define S9000_ID_TIMBER		0x27F12392	/* Bushmaster (710) Graphics */
#define S9000_ID_TOMCAT		0x27FCCB6D	/* dual-headed ELK (Dual CRX) */
#define S9000_ID_ARTIST		0x2B4DED6D	/* Artist (Gecko/712 & 715) onboard Graphics */
#define S9000_ID_HCRX		0x2BCB015A	/* Hyperdrive/Hyperbowl (A4071A) Graphics */
#define CRX24_OVERLAY_PLANES	0x920825AA	/* Overlay planes on CRX24 */

#define CRT_ID_ELK_1024		S9000_ID_ELK_768 /* Elk 1024x768  CRX */
#define CRT_ID_ELK_1280		S9000_ID_A1659A	/* Elk 1280x1024 CRX */
#define CRT_ID_ELK_1024DB	0x27849CA5      /* Elk 1024x768 double buffer */
#define CRT_ID_ELK_GS		S9000_ID_A1924A	/* Elk 1280x1024 GreyScale    */
#define CRT_ID_CRX24		S9000_ID_A1439A	/* Piranha */
#define CRT_ID_VISUALIZE_EG	0x2D08C0A7      /* Graffiti, A4450A (built-in B132+/B160L) */
#define CRT_ID_THUNDER		0x2F23E5FC      /* Thunder 1 VISUALIZE 48*/
#define CRT_ID_THUNDER2		0x2F8D570E      /* Thunder 2 VISUALIZE 48 XP*/
#define CRT_ID_HCRX		S9000_ID_HCRX	/* Hyperdrive HCRX */
#define CRT_ID_CRX48Z		S9000_ID_STINGER /* Stinger */
#define CRT_ID_DUAL_CRX		S9000_ID_TOMCAT	/* Tomcat */
#define CRT_ID_PVRX		S9000_ID_98705	/* Tigershark */
#define CRT_ID_TIMBER		S9000_ID_TIMBER	/* Timber (710 builtin) */
#define CRT_ID_TVRX		S9000_ID_98765	/* TVRX (gto/falcon) */
#define CRT_ID_ARTIST		S9000_ID_ARTIST	/* Artist */
#define CRT_ID_SUMMIT		0x2FC1066B      /* Summit FX2, FX4, FX6 ... */
#define CRT_ID_LEGO		0x35ACDA30	/* Lego FX5, FX10 ... */
#define CRT_ID_PINNACLE		0x35ACDA16	/* Pinnacle FXe */ 

#endif /* __ASM_PARISC_GRFIOCTL_H */
