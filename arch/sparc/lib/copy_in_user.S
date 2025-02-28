/* SPDX-License-Identifier: GPL-2.0 */
/* copy_in_user.S: Copy from userspace to userspace.
 *
 * Copyright (C) 1999, 2000, 2004 David S. Miller (davem@redhat.com)
 */

#include <freax/export.h>
#include <freax/linkage.h>
#include <asm/asi.h>

#define XCC xcc

#define EX(x,y,z)		\
98:	x,y;			\
	.section __ex_table,"a";\
	.align 4;		\
	.word 98b, z;		\
	.text;			\
	.align 4;

#define EX_O4(x,y) EX(x,y,__retl_o4_plus_8)
#define EX_O2_4(x,y) EX(x,y,__retl_o2_plus_4)
#define EX_O2_1(x,y) EX(x,y,__retl_o2_plus_1)

	.register	%g2,#scratch
	.register	%g3,#scratch

	.text
__retl_o4_plus_8:
	add	%o4, %o2, %o4
	retl
	 add	%o4, 8, %o0
__retl_o2_plus_4:
	retl
	 add	%o2, 4, %o0
__retl_o2_plus_1:
	retl
	 add	%o2, 1, %o0

	.align	32

	/* Don't try to get too fancy here, just nice and
	 * simple.  This is predominantly used for well aligned
	 * small copies in the compat layer.  It is also used
	 * to copy register windows around during thread cloning.
	 */

ENTRY(raw_copy_in_user)	/* %o0=dst, %o1=src, %o2=len */
	cmp		%o2, 0
	be,pn		%XCC, 85f
	 or		%o0, %o1, %o3
	cmp		%o2, 16
	bleu,a,pn	%XCC, 80f
	 or		%o3, %o2, %o3

	/* 16 < len <= 64 */
	andcc		%o3, 0x7, %g0
	bne,pn		%XCC, 90f
	 nop

	andn		%o2, 0x7, %o4
	and		%o2, 0x7, %o2
1:	subcc		%o4, 0x8, %o4
	EX_O4(ldxa [%o1] %asi, %o5)
	EX_O4(stxa %o5, [%o0] %asi)
	add		%o1, 0x8, %o1
	bgu,pt		%XCC, 1b
	 add		%o0, 0x8, %o0
	andcc		%o2, 0x4, %g0
	be,pt		%XCC, 1f
	 nop
	sub		%o2, 0x4, %o2
	EX_O2_4(lduwa [%o1] %asi, %o5)
	EX_O2_4(stwa %o5, [%o0] %asi)
	add		%o1, 0x4, %o1
	add		%o0, 0x4, %o0
1:	cmp		%o2, 0
	be,pt		%XCC, 85f
	 nop
	ba,pt		%xcc, 90f
	 nop

80:	/* 0 < len <= 16 */
	andcc		%o3, 0x3, %g0
	bne,pn		%XCC, 90f
	 nop

82:
	subcc		%o2, 4, %o2
	EX_O2_4(lduwa [%o1] %asi, %g1)
	EX_O2_4(stwa %g1, [%o0] %asi)
	add		%o1, 4, %o1
	bgu,pt		%XCC, 82b
	 add		%o0, 4, %o0

85:	retl
	 clr		%o0

	.align	32
90:
	subcc		%o2, 1, %o2
	EX_O2_1(lduba [%o1] %asi, %g1)
	EX_O2_1(stba %g1, [%o0] %asi)
	add		%o1, 1, %o1
	bgu,pt		%XCC, 90b
	 add		%o0, 1, %o0
	retl
	 clr		%o0
ENDPROC(raw_copy_in_user)
EXPORT_SYMBOL(raw_copy_in_user)
