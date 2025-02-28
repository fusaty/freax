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
 *	@(#)	pa/fp/decode_exc.c		$ Revision: $
 *
 *  Purpose:
 *	<<please update with a synopsis of the functionality provided by this file>>
 *
 *  External Interfaces:
 *	<<the following list was autogenerated, please review>>
 *	decode_fpu(Fpu_register, trap_counts)
 *
 *  Internal Interfaces:
 *	<<please update>>
 *
 *  Theory:
 *	<<please update with a overview of the operation of this file>>
 *
 * END_DESC
*/

#include <freax/kernel.h>
#include "float.h"
#include "sgl_float.h"
#include "dbl_float.h"
#include "cnv_float.h"
/* #include "types.h" */
#include <asm/signal.h>
#include <asm/siginfo.h>
/* #include <machine/sys/mdep_private.h> */

#undef Fpustatus_register
#define Fpustatus_register Fpu_register[0]

/* General definitions */
#define DOESTRAP 1
#define NOTRAP 0
#define SIGNALCODE(signal, code) ((signal) << 24 | (code))
#define copropbit	1<<31-2	/* bit position 2 */
#define opclass		9	/* bits 21 & 22 */
#define fmtbits		11	/* bits 19 & 20 */
#define df		13	/* bits 17 & 18 */
#define twobits		3	/* mask low-order 2 bits */
#define fivebits	31	/* mask low-order 5 bits */
#define MAX_EXCP_REG	7	/* number of excpeption registers to check */

/* Exception register definitions */
#define Excp_type(index) Exceptiontype(Fpu_register[index])
#define Excp_instr(index) Instructionfield(Fpu_register[index])
#define Clear_excp_register(index) Allexception(Fpu_register[index]) = 0
#define Excp_format() \
	(current_ir >> ((current_ir>>opclass & twobits) == 1 ? df : fmtbits) & twobits)

/* Miscellaneous definitions */
#define Fpu_sgl(index) Fpu_register[index*2]

#define Fpu_dblp1(index) Fpu_register[index*2]
#define Fpu_dblp2(index) Fpu_register[(index*2)+1]

#define Fpu_quadp1(index) Fpu_register[index*2]
#define Fpu_quadp2(index) Fpu_register[(index*2)+1]
#define Fpu_quadp3(index) Fpu_register[(index*2)+2]
#define Fpu_quadp4(index) Fpu_register[(index*2)+3]

/* Single precision floating-point definitions */
#ifndef Sgl_decrement
# define Sgl_decrement(sgl_value) Sall(sgl_value)--
#endif

/* Double precision floating-point definitions */
#ifndef Dbl_decrement
# define Dbl_decrement(dbl_valuep1,dbl_valuep2) \
    if ((Dallp2(dbl_valuep2)--) == 0) Dallp1(dbl_valuep1)-- 
#endif


#define update_trap_counts(Fpu_register, aflags, bflags, trap_counts) {	\
	aflags=(Fpu_register[0])>>27;	/* assumes zero fill. 32 bit */	\
	Fpu_register[0] |= bflags;					\
}

u_int
decode_fpu(unsigned int Fpu_register[], unsigned int trap_counts[])
{
    unsigned int current_ir, excp;
    int target, exception_index = 1;
    boolean inexact;
    unsigned int aflags;
    unsigned int bflags;
    unsigned int excptype;


    /* Keep stats on how many floating point exceptions (based on type)
     * that happen.  Want to keep this overhead low, but still provide
     * some information to the customer.  All exits from this routine
     * need to restore Fpu_register[0]
     */

    bflags=(Fpu_register[0] & 0xf8000000);
    Fpu_register[0] &= 0x07ffffff;

    /* exception_index is used to index the exception register queue.  It
     *   always points at the last register that contains a valid exception.  A
     *   zero value implies no exceptions (also the initialized value).  Setting
     *   the T-bit resets the exception_index to zero.
     */

    /*
     * Check for reserved-op exception.  A reserved-op exception does not 
     * set any exception registers nor does it set the T-bit.  If the T-bit
     * is not set then a reserved-op exception occurred.
     *
     * At some point, we may want to report reserved op exceptions as
     * illegal instructions.
     */
    
    if (!Is_tbit_set()) {
	update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
	return SIGNALCODE(SIGILL, ILL_COPROC);
    }

    /* 
     * Is a coprocessor op. 
     *
     * Now we need to determine what type of exception occurred.
     */
    for (exception_index=1; exception_index<=MAX_EXCP_REG; exception_index++) {
	current_ir = Excp_instr(exception_index);
	  /*
	   * On PA89: there are 5 different unimplemented exception
	   * codes: 0x1, 0x9, 0xb, 0x3, and 0x23.  PA-RISC 2.0 adds
	   * another, 0x2b.  Only these have the low order bit set.
	   */
	excptype = Excp_type(exception_index);
	if (excptype & UNIMPLEMENTEDEXCEPTION) {
		/*
		 * Clear T-bit and exception register so that
		 * we can tell if a trap really occurs while 
		 * emulating the instruction.
		 */
		Clear_tbit();
		Clear_excp_register(exception_index);
		/*
		 * Now emulate this instruction.  If a trap occurs,
		 * fpudispatch will return a non-zero number 
		 */
		excp = fpudispatch(current_ir,excptype,0,Fpu_register);
		/* accumulate the status flags, don't lose them as in hpux */
		if (excp) {
			/*
			 * We now need to make sure that the T-bit and the
			 * exception register contain the correct values
			 * before continuing.
			 */
			/*
			 * Set t-bit since it might still be needed for a
			 * subsequent real trap (I don't understand fully -PB)
			 */
			Set_tbit();
			/* some of the following code uses
			 * Excp_type(exception_index) so fix that up */
			Set_exceptiontype_and_instr_field(excp,current_ir,
			 Fpu_register[exception_index]);
			if (excp == UNIMPLEMENTEDEXCEPTION) {
				/*
			 	 * it is really unimplemented, so restore the
			 	 * TIMEX extended unimplemented exception code
			 	 */
				excp = excptype;
				update_trap_counts(Fpu_register, aflags, bflags, 
					   trap_counts);
				return SIGNALCODE(SIGILL, ILL_COPROC);
			}
			/* some of the following code uses excptype, so
			 * fix that up too */
			excptype = excp;
		}
		/* handle exceptions other than the real UNIMPLIMENTED the
		 * same way as if the hardware had caused them */
		if (excp == NOEXCEPTION)
			/* For now use 'break', should technically be 'continue' */
			break;
	}

	  /*
	   * In PA89, the underflow exception has been extended to encode
	   * additional information.  The exception looks like pp01x0,
	   * where x is 1 if inexact and pp represent the inexact bit (I)
	   * and the round away bit (RA)
	   */
	if (excptype & UNDERFLOWEXCEPTION) {
		/* check for underflow trap enabled */
		if (Is_underflowtrap_enabled()) {
			update_trap_counts(Fpu_register, aflags, bflags, 
					   trap_counts);
			return SIGNALCODE(SIGFPE, FPE_FLTUND);
		} else {
		    /*
		     * Isn't a real trap; we need to 
		     * return the default value.
		     */
		    target = current_ir & fivebits;
#ifndef lint
		    if (Ibit(Fpu_register[exception_index])) inexact = TRUE;
		    else inexact = FALSE;
#endif
		    switch (Excp_format()) {
		      case SGL:
		        /*
		         * If ra (round-away) is set, will 
		         * want to undo the rounding done
		         * by the hardware.
		         */
		        if (Rabit(Fpu_register[exception_index])) 
				Sgl_decrement(Fpu_sgl(target));

			/* now denormalize */
			sgl_denormalize(&Fpu_sgl(target),&inexact,Rounding_mode());
		    	break;
		      case DBL:
		    	/*
		    	 * If ra (round-away) is set, will 
		    	 * want to undo the rounding done
		    	 * by the hardware.
		    	 */
		    	if (Rabit(Fpu_register[exception_index])) 
				Dbl_decrement(Fpu_dblp1(target),Fpu_dblp2(target));

			/* now denormalize */
			dbl_denormalize(&Fpu_dblp1(target),&Fpu_dblp2(target),
			  &inexact,Rounding_mode());
		    	break;
		    }
		    if (inexact) Set_underflowflag();
		    /* 
		     * Underflow can generate an inexact
		     * exception.  If inexact trap is enabled,
		     * want to do an inexact trap, otherwise 
		     * set inexact flag.
		     */
		    if (inexact && Is_inexacttrap_enabled()) {
		    	/*
		    	 * Set exception field of exception register
		    	 * to inexact, parm field to zero.
			 * Underflow bit should be cleared.
		    	 */
		    	Set_exceptiontype(Fpu_register[exception_index],
			 INEXACTEXCEPTION);
			Set_parmfield(Fpu_register[exception_index],0);
			update_trap_counts(Fpu_register, aflags, bflags, 
					   trap_counts);
			return SIGNALCODE(SIGFPE, FPE_FLTRES);
		    }
		    else {
		    	/*
		    	 * Exception register needs to be cleared.  
			 * Inexact flag needs to be set if inexact.
		    	 */
		    	Clear_excp_register(exception_index);
		    	if (inexact) Set_inexactflag();
		    }
		}
		continue;
	}
	switch(Excp_type(exception_index)) {
	  case OVERFLOWEXCEPTION:
	  case OVERFLOWEXCEPTION | INEXACTEXCEPTION:
		/* check for overflow trap enabled */
			update_trap_counts(Fpu_register, aflags, bflags, 
					   trap_counts);
		if (Is_overflowtrap_enabled()) {
			update_trap_counts(Fpu_register, aflags, bflags, 
					   trap_counts);
			return SIGNALCODE(SIGFPE, FPE_FLTOVF);
		} else {
			/*
			 * Isn't a real trap; we need to 
			 * return the default value.
			 */
			target = current_ir & fivebits;
			switch (Excp_format()) {
			  case SGL: 
				Sgl_setoverflow(Fpu_sgl(target));
				break;
			  case DBL:
				Dbl_setoverflow(Fpu_dblp1(target),Fpu_dblp2(target));
				break;
			}
			Set_overflowflag();
			/* 
			 * Overflow always generates an inexact
			 * exception.  If inexact trap is enabled,
			 * want to do an inexact trap, otherwise 
			 * set inexact flag.
			 */
			if (Is_inexacttrap_enabled()) {
				/*
				 * Set exception field of exception
				 * register to inexact.  Overflow
				 * bit should be cleared.
				 */
				Set_exceptiontype(Fpu_register[exception_index],
				 INEXACTEXCEPTION);
				update_trap_counts(Fpu_register, aflags, bflags,
					   trap_counts);
				return SIGNALCODE(SIGFPE, FPE_FLTRES);
			}
			else {
				/*
				 * Exception register needs to be cleared.  
				 * Inexact flag needs to be set.
				 */
				Clear_excp_register(exception_index);
				Set_inexactflag();
			}
		}
		break;
	  case INVALIDEXCEPTION:
	  case OPC_2E_INVALIDEXCEPTION:
		update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
		return SIGNALCODE(SIGFPE, FPE_FLTINV);
	  case DIVISIONBYZEROEXCEPTION:
		update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
		Clear_excp_register(exception_index);
	  	return SIGNALCODE(SIGFPE, FPE_FLTDIV);
	  case INEXACTEXCEPTION:
		update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
		return SIGNALCODE(SIGFPE, FPE_FLTRES);
	  default:
		update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
		printk("%s(%d) Unknown FPU exception 0x%x\n", __FILE__,
			__LINE__, Excp_type(exception_index));
		return SIGNALCODE(SIGILL, ILL_COPROC);
	  case NOEXCEPTION:	/* no exception */
		/*
		 * Clear exception register in case 
		 * other fields are non-zero.
		 */
		Clear_excp_register(exception_index);
		break;
	}
    }
    /*
     * No real exceptions occurred.
     */
    Clear_tbit();
    update_trap_counts(Fpu_register, aflags, bflags, trap_counts);
    return(NOTRAP);
}
