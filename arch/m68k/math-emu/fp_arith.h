/* SPDX-License-Identifier: GPL-2.0-or-later */
/*

   fp_arith.h: floating-point math routines for the freax-m68k
   floating point emulator.

   Copyright (c) 1998 David Huggins-Daines.

   Somewhat based on the Alphafreax floating point emulator, by David
   Mosberger-Tang.


 */

#ifndef _FP_ARITH_H
#define _FP_ARITH_H

/* easy ones */
struct fp_ext *fp_fabs(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fneg(struct fp_ext *dest, struct fp_ext *src);

/* straightforward arithmetic */
struct fp_ext *fp_fadd(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fsub(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fcmp(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_ftst(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fmul(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fdiv(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fsglmul(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fsgldiv(struct fp_ext *dest, struct fp_ext *src);

/* ones that do rounding and integer conversions */
struct fp_ext *fp_fmod(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_frem(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fint(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fintrz(struct fp_ext *dest, struct fp_ext *src);
struct fp_ext *fp_fscale(struct fp_ext *dest, struct fp_ext *src);

#endif	/* _FP_ARITH_H */
