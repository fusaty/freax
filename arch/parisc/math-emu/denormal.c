// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * freax/PA-RISC Project (http://www.parisc-freax.org/)
 *
 * Floating-point emulation code
 *  Copyright (C) 2001 Hewlett-Packard (Paul Bame) <bame@debian.org>
 */
/*
 * BEGIN_DESC
 *
 *  File:
 *	@(#)	pa/fp/denormal.c		$ Revision: $
 *
 *  Purpose:
 *	<<please update with a synopsis of the functionality provided by this file>>
 *
 *  External Interfaces:
 *	<<the following list was autogenerated, please review>>
 *	dbl_denormalize(dbl_opndp1,dbl_opndp2,inexactflag,rmode)
 *	sgl_denormalize(sgl_opnd,inexactflag,rmode)
 *
 *  Internal Interfaces:
 *	<<please update>>
 *
 *  Theory:
 *	<<please update with a overview of the operation of this file>>
 *
 * END_DESC
*/



#include "float.h"
#include "sgl_float.h"
#include "dbl_float.h"
#include "hppa.h"
#include <freax/kernel.h>
/* #include <machine/sys/mdep_private.h> */

#undef Fpustatus_register
#define Fpustatus_register Fpu_register[0]

void
sgl_denormalize(unsigned int *sgl_opnd, boolean *inexactflag, int rmode)
{
	unsigned int opnd;
	int sign, exponent;
	boolean guardbit = FALSE, stickybit, inexact;

	opnd = *sgl_opnd;
	stickybit = *inexactflag;
        exponent = Sgl_exponent(opnd) - SGL_WRAP;
        sign = Sgl_sign(opnd);
	Sgl_denormalize(opnd,exponent,guardbit,stickybit,inexact);
	if (inexact) {
	    switch (rmode) {
	      case ROUNDPLUS:
		if (sign == 0) {
			Sgl_increment(opnd);
		}
		break;
	      case ROUNDMINUS:
		if (sign != 0) {
			Sgl_increment(opnd);
		}
		break;
	      case ROUNDNEAREST:
		if (guardbit && (stickybit || 
		       Sgl_isone_lowmantissa(opnd))) {
			   Sgl_increment(opnd);
		}
		break;
	    }
	}
	Sgl_set_sign(opnd,sign);
	*sgl_opnd = opnd;
	*inexactflag = inexact;
	return;
}

void
dbl_denormalize(unsigned int *dbl_opndp1,
	unsigned int * dbl_opndp2,
	boolean *inexactflag,
	int rmode)
{
	unsigned int opndp1, opndp2;
	int sign, exponent;
	boolean guardbit = FALSE, stickybit, inexact;

	opndp1 = *dbl_opndp1;
	opndp2 = *dbl_opndp2;
	stickybit = *inexactflag;
	exponent = Dbl_exponent(opndp1) - DBL_WRAP;
	sign = Dbl_sign(opndp1);
	Dbl_denormalize(opndp1,opndp2,exponent,guardbit,stickybit,inexact);
	if (inexact) {
	    switch (rmode) {
	      case ROUNDPLUS:
		if (sign == 0) {
			Dbl_increment(opndp1,opndp2);
		}
		break;
	      case ROUNDMINUS:
		if (sign != 0) {
			Dbl_increment(opndp1,opndp2);
		}
		break;
	      case ROUNDNEAREST:
		if (guardbit && (stickybit || 
		       Dbl_isone_lowmantissap2(opndp2))) {
			   Dbl_increment(opndp1,opndp2);
		}
		break;
	    }
	}
	Dbl_set_sign(opndp1,sign);
	*dbl_opndp1 = opndp1;
	*dbl_opndp2 = opndp2;
	*inexactflag = inexact;
	return;
}
