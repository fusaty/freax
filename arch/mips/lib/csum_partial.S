/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Quick'n'dirty IP checksum ...
 *
 * Copyright (C) 1998, 1999 Ralf Baechle
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Copyright (C) 2007  Maciej W. Rozycki
 * Copyright (C) 2014 Imagination Technologies Ltd.
 */
#include <freax/errno.h>
#include <freax/export.h>
#include <asm/asm.h>
#include <asm/asm-offsets.h>
#include <asm/regdef.h>

#ifdef CONFIG_64BIT
/*
 * As we are sharing code base with the mips32 tree (which use the o32 ABI
 * register definitions). We need to redefine the register definitions from
 * the n64 ABI register naming to the o32 ABI register naming.
 */
#undef t0
#undef t1
#undef t2
#undef t3
#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15

#define USE_DOUBLE
#endif

#ifdef USE_DOUBLE

#define LOAD   ld
#define LOAD32 lwu
#define ADD    daddu
#define NBYTES 8

#else

#define LOAD   lw
#define LOAD32 lw
#define ADD    addu
#define NBYTES 4

#endif /* USE_DOUBLE */

#define UNIT(unit)  ((unit)*NBYTES)

#define ADDC(sum,reg)						\
	.set	push;						\
	.set	noat;						\
	ADD	sum, reg;					\
	sltu	v1, sum, reg;					\
	ADD	sum, v1;					\
	.set	pop

#define ADDC32(sum,reg)						\
	.set	push;						\
	.set	noat;						\
	addu	sum, reg;					\
	sltu	v1, sum, reg;					\
	addu	sum, v1;					\
	.set	pop

#define CSUM_BIGCHUNK1(src, offset, sum, _t0, _t1, _t2, _t3)	\
	LOAD	_t0, (offset + UNIT(0))(src);			\
	LOAD	_t1, (offset + UNIT(1))(src);			\
	LOAD	_t2, (offset + UNIT(2))(src);			\
	LOAD	_t3, (offset + UNIT(3))(src);			\
	ADDC(_t0, _t1);						\
	ADDC(_t2, _t3);						\
	ADDC(sum, _t0);						\
	ADDC(sum, _t2)

#ifdef USE_DOUBLE
#define CSUM_BIGCHUNK(src, offset, sum, _t0, _t1, _t2, _t3)	\
	CSUM_BIGCHUNK1(src, offset, sum, _t0, _t1, _t2, _t3)
#else
#define CSUM_BIGCHUNK(src, offset, sum, _t0, _t1, _t2, _t3)	\
	CSUM_BIGCHUNK1(src, offset, sum, _t0, _t1, _t2, _t3);	\
	CSUM_BIGCHUNK1(src, offset + 0x10, sum, _t0, _t1, _t2, _t3)
#endif

/*
 * a0: source address
 * a1: length of the area to checksum
 * a2: partial checksum
 */

#define src a0
#define sum v0

	.text
	.set	noreorder
	.align	5
LEAF(csum_partial)
EXPORT_SYMBOL(csum_partial)
	move	sum, zero
	move	t7, zero

	sltiu	t8, a1, 0x8
	bnez	t8, .Lsmall_csumcpy		/* < 8 bytes to copy */
	 move	t2, a1

	andi	t7, src, 0x1			/* odd buffer? */

.Lhword_align:
	beqz	t7, .Lword_align
	 andi	t8, src, 0x2

	lbu	t0, (src)
	LONG_SUBU	a1, a1, 0x1
#ifdef __MIPSEL__
	sll	t0, t0, 8
#endif
	ADDC(sum, t0)
	PTR_ADDU	src, src, 0x1
	andi	t8, src, 0x2

.Lword_align:
	beqz	t8, .Ldword_align
	 sltiu	t8, a1, 56

	lhu	t0, (src)
	LONG_SUBU	a1, a1, 0x2
	ADDC(sum, t0)
	sltiu	t8, a1, 56
	PTR_ADDU	src, src, 0x2

.Ldword_align:
	bnez	t8, .Ldo_end_words
	 move	t8, a1

	andi	t8, src, 0x4
	beqz	t8, .Lqword_align
	 andi	t8, src, 0x8

	LOAD32	t0, 0x00(src)
	LONG_SUBU	a1, a1, 0x4
	ADDC(sum, t0)
	PTR_ADDU	src, src, 0x4
	andi	t8, src, 0x8

.Lqword_align:
	beqz	t8, .Loword_align
	 andi	t8, src, 0x10

#ifdef USE_DOUBLE
	ld	t0, 0x00(src)
	LONG_SUBU	a1, a1, 0x8
	ADDC(sum, t0)
#else
	lw	t0, 0x00(src)
	lw	t1, 0x04(src)
	LONG_SUBU	a1, a1, 0x8
	ADDC(sum, t0)
	ADDC(sum, t1)
#endif
	PTR_ADDU	src, src, 0x8
	andi	t8, src, 0x10

.Loword_align:
	beqz	t8, .Lbegin_movement
	 LONG_SRL	t8, a1, 0x7

#ifdef USE_DOUBLE
	ld	t0, 0x00(src)
	ld	t1, 0x08(src)
	ADDC(sum, t0)
	ADDC(sum, t1)
#else
	CSUM_BIGCHUNK1(src, 0x00, sum, t0, t1, t3, t4)
#endif
	LONG_SUBU	a1, a1, 0x10
	PTR_ADDU	src, src, 0x10
	LONG_SRL	t8, a1, 0x7

.Lbegin_movement:
	beqz	t8, 1f
	 andi	t2, a1, 0x40

.Lmove_128bytes:
	CSUM_BIGCHUNK(src, 0x00, sum, t0, t1, t3, t4)
	CSUM_BIGCHUNK(src, 0x20, sum, t0, t1, t3, t4)
	CSUM_BIGCHUNK(src, 0x40, sum, t0, t1, t3, t4)
	CSUM_BIGCHUNK(src, 0x60, sum, t0, t1, t3, t4)
	LONG_SUBU	t8, t8, 0x01
	.set	reorder				/* DADDI_WAR */
	PTR_ADDU	src, src, 0x80
	bnez	t8, .Lmove_128bytes
	.set	noreorder

1:
	beqz	t2, 1f
	 andi	t2, a1, 0x20

.Lmove_64bytes:
	CSUM_BIGCHUNK(src, 0x00, sum, t0, t1, t3, t4)
	CSUM_BIGCHUNK(src, 0x20, sum, t0, t1, t3, t4)
	PTR_ADDU	src, src, 0x40

1:
	beqz	t2, .Ldo_end_words
	 andi	t8, a1, 0x1c

.Lmove_32bytes:
	CSUM_BIGCHUNK(src, 0x00, sum, t0, t1, t3, t4)
	andi	t8, a1, 0x1c
	PTR_ADDU	src, src, 0x20

.Ldo_end_words:
	beqz	t8, .Lsmall_csumcpy
	 andi	t2, a1, 0x3
	LONG_SRL	t8, t8, 0x2

.Lend_words:
	LOAD32	t0, (src)
	LONG_SUBU	t8, t8, 0x1
	ADDC(sum, t0)
	.set	reorder				/* DADDI_WAR */
	PTR_ADDU	src, src, 0x4
	bnez	t8, .Lend_words
	.set	noreorder

/* unknown src alignment and < 8 bytes to go  */
.Lsmall_csumcpy:
	move	a1, t2

	andi	t0, a1, 4
	beqz	t0, 1f
	 andi	t0, a1, 2

	/* Still a full word to go  */
	ulw	t1, (src)
	PTR_ADDIU	src, 4
#ifdef USE_DOUBLE
	dsll	t1, t1, 32			/* clear lower 32bit */
#endif
	ADDC(sum, t1)

1:	move	t1, zero
	beqz	t0, 1f
	 andi	t0, a1, 1

	/* Still a halfword to go  */
	ulhu	t1, (src)
	PTR_ADDIU	src, 2

1:	beqz	t0, 1f
	 sll	t1, t1, 16

	lbu	t2, (src)
	 nop

#ifdef __MIPSEB__
	sll	t2, t2, 8
#endif
	or	t1, t2

1:	ADDC(sum, t1)

	/* fold checksum */
#ifdef USE_DOUBLE
	dsll32	v1, sum, 0
	daddu	sum, v1
	sltu	v1, sum, v1
	dsra32	sum, sum, 0
	addu	sum, v1
#endif

	/* odd buffer alignment? */
#if defined(CONFIG_CPU_MIPSR2) || defined(CONFIG_CPU_MIPSR5) || \
    defined(CONFIG_CPU_LOONGSON64)
	.set	push
	.set	arch=mips32r2
	wsbh	v1, sum
	movn	sum, v1, t7
	.set	pop
#else
	beqz	t7, 1f			/* odd buffer alignment? */
	 lui	v1, 0x00ff
	addu	v1, 0x00ff
	and	t0, sum, v1
	sll	t0, t0, 8
	srl	sum, sum, 8
	and	sum, sum, v1
	or	sum, sum, t0
1:
#endif
	.set	reorder
	/* Add the passed partial csum.	 */
	ADDC32(sum, a2)
	jr	ra
	.set	noreorder
	END(csum_partial)


/*
 * checksum and copy routines based on memcpy.S
 *
 *	csum_partial_copy_nocheck(src, dst, len)
 *	__csum_partial_copy_kernel(src, dst, len)
 *
 * See "Spec" in memcpy.S for details.	Unlike __copy_user, all
 * function in this file use the standard calling convention.
 */

#define src a0
#define dst a1
#define len a2
#define sum v0
#define odd t8

/*
 * All exception handlers simply return 0.
 */

/* Instruction type */
#define LD_INSN 1
#define ST_INSN 2
#define LEGACY_MODE 1
#define EVA_MODE    2
#define USEROP   1
#define KERNELOP 2

/*
 * Wrapper to add an entry in the exception table
 * in case the insn causes a memory exception.
 * Arguments:
 * insn    : Load/store instruction
 * type    : Instruction type
 * reg     : Register
 * addr    : Address
 * handler : Exception handler
 */
#define EXC(insn, type, reg, addr)		\
	.if \mode == LEGACY_MODE;		\
9:		insn reg, addr;			\
		.section __ex_table,"a";	\
		PTR_WD	9b, .L_exc;		\
		.previous;			\
	/* This is enabled in EVA mode */	\
	.else;					\
		/* If loading from user or storing to user */	\
		.if ((\from == USEROP) && (type == LD_INSN)) || \
		    ((\to == USEROP) && (type == ST_INSN));	\
9:			__BUILD_EVA_INSN(insn##e, reg, addr);	\
			.section __ex_table,"a";		\
			PTR_WD	9b, .L_exc;			\
			.previous;				\
		.else;						\
			/* EVA without exception */		\
			insn reg, addr;				\
		.endif;						\
	.endif

#undef LOAD

#ifdef USE_DOUBLE

#define LOADK	ld /* No exception */
#define LOAD(reg, addr)		EXC(ld, LD_INSN, reg, addr)
#define LOADBU(reg, addr)	EXC(lbu, LD_INSN, reg, addr)
#define LOADL(reg, addr)	EXC(ldl, LD_INSN, reg, addr)
#define LOADR(reg, addr)	EXC(ldr, LD_INSN, reg, addr)
#define STOREB(reg, addr)	EXC(sb, ST_INSN, reg, addr)
#define STOREL(reg, addr)	EXC(sdl, ST_INSN, reg, addr)
#define STORER(reg, addr)	EXC(sdr, ST_INSN, reg, addr)
#define STORE(reg, addr)	EXC(sd, ST_INSN, reg, addr)
#define ADD    daddu
#define SUB    dsubu
#define SRL    dsrl
#define SLL    dsll
#define SLLV   dsllv
#define SRLV   dsrlv
#define NBYTES 8
#define LOG_NBYTES 3

#else

#define LOADK	lw /* No exception */
#define LOAD(reg, addr)		EXC(lw, LD_INSN, reg, addr)
#define LOADBU(reg, addr)	EXC(lbu, LD_INSN, reg, addr)
#define LOADL(reg, addr)	EXC(lwl, LD_INSN, reg, addr)
#define LOADR(reg, addr)	EXC(lwr, LD_INSN, reg, addr)
#define STOREB(reg, addr)	EXC(sb, ST_INSN, reg, addr)
#define STOREL(reg, addr)	EXC(swl, ST_INSN, reg, addr)
#define STORER(reg, addr)	EXC(swr, ST_INSN, reg, addr)
#define STORE(reg, addr)	EXC(sw, ST_INSN, reg, addr)
#define ADD    addu
#define SUB    subu
#define SRL    srl
#define SLL    sll
#define SLLV   sllv
#define SRLV   srlv
#define NBYTES 4
#define LOG_NBYTES 2

#endif /* USE_DOUBLE */

#ifdef CONFIG_CPU_LITTLE_ENDIAN
#define LDFIRST LOADR
#define LDREST	LOADL
#define STFIRST STORER
#define STREST	STOREL
#define SHIFT_DISCARD SLLV
#define SHIFT_DISCARD_REVERT SRLV
#else
#define LDFIRST LOADL
#define LDREST	LOADR
#define STFIRST STOREL
#define STREST	STORER
#define SHIFT_DISCARD SRLV
#define SHIFT_DISCARD_REVERT SLLV
#endif

#define FIRST(unit) ((unit)*NBYTES)
#define REST(unit)  (FIRST(unit)+NBYTES-1)

#define ADDRMASK (NBYTES-1)

#ifndef CONFIG_CPU_DADDI_WORKAROUNDS
	.set	noat
#else
	.set	at=v1
#endif

	.macro __BUILD_CSUM_PARTIAL_COPY_USER mode, from, to

	li	sum, -1
	move	odd, zero
	/*
	 * Note: dst & src may be unaligned, len may be 0
	 * Temps
	 */
	/*
	 * The "issue break"s below are very approximate.
	 * Issue delays for dcache fills will perturb the schedule, as will
	 * load queue full replay traps, etc.
	 *
	 * If len < NBYTES use byte operations.
	 */
	sltu	t2, len, NBYTES
	and	t1, dst, ADDRMASK
	bnez	t2, .Lcopy_bytes_checklen\@
	 and	t0, src, ADDRMASK
	andi	odd, dst, 0x1			/* odd buffer? */
	bnez	t1, .Ldst_unaligned\@
	 nop
	bnez	t0, .Lsrc_unaligned_dst_aligned\@
	/*
	 * use delay slot for fall-through
	 * src and dst are aligned; need to compute rem
	 */
.Lboth_aligned\@:
	 SRL	t0, len, LOG_NBYTES+3	 # +3 for 8 units/iter
	beqz	t0, .Lcleanup_both_aligned\@ # len < 8*NBYTES
	 nop
	SUB	len, 8*NBYTES		# subtract here for bgez loop
	.align	4
1:
	LOAD(t0, UNIT(0)(src))
	LOAD(t1, UNIT(1)(src))
	LOAD(t2, UNIT(2)(src))
	LOAD(t3, UNIT(3)(src))
	LOAD(t4, UNIT(4)(src))
	LOAD(t5, UNIT(5)(src))
	LOAD(t6, UNIT(6)(src))
	LOAD(t7, UNIT(7)(src))
	SUB	len, len, 8*NBYTES
	ADD	src, src, 8*NBYTES
	STORE(t0, UNIT(0)(dst))
	ADDC(t0, t1)
	STORE(t1, UNIT(1)(dst))
	ADDC(sum, t0)
	STORE(t2, UNIT(2)(dst))
	ADDC(t2, t3)
	STORE(t3, UNIT(3)(dst))
	ADDC(sum, t2)
	STORE(t4, UNIT(4)(dst))
	ADDC(t4, t5)
	STORE(t5, UNIT(5)(dst))
	ADDC(sum, t4)
	STORE(t6, UNIT(6)(dst))
	ADDC(t6, t7)
	STORE(t7, UNIT(7)(dst))
	ADDC(sum, t6)
	.set	reorder				/* DADDI_WAR */
	ADD	dst, dst, 8*NBYTES
	bgez	len, 1b
	.set	noreorder
	ADD	len, 8*NBYTES		# revert len (see above)

	/*
	 * len == the number of bytes left to copy < 8*NBYTES
	 */
.Lcleanup_both_aligned\@:
#define rem t7
	beqz	len, .Ldone\@
	 sltu	t0, len, 4*NBYTES
	bnez	t0, .Lless_than_4units\@
	 and	rem, len, (NBYTES-1)	# rem = len % NBYTES
	/*
	 * len >= 4*NBYTES
	 */
	LOAD(t0, UNIT(0)(src))
	LOAD(t1, UNIT(1)(src))
	LOAD(t2, UNIT(2)(src))
	LOAD(t3, UNIT(3)(src))
	SUB	len, len, 4*NBYTES
	ADD	src, src, 4*NBYTES
	STORE(t0, UNIT(0)(dst))
	ADDC(t0, t1)
	STORE(t1, UNIT(1)(dst))
	ADDC(sum, t0)
	STORE(t2, UNIT(2)(dst))
	ADDC(t2, t3)
	STORE(t3, UNIT(3)(dst))
	ADDC(sum, t2)
	.set	reorder				/* DADDI_WAR */
	ADD	dst, dst, 4*NBYTES
	beqz	len, .Ldone\@
	.set	noreorder
.Lless_than_4units\@:
	/*
	 * rem = len % NBYTES
	 */
	beq	rem, len, .Lcopy_bytes\@
	 nop
1:
	LOAD(t0, 0(src))
	ADD	src, src, NBYTES
	SUB	len, len, NBYTES
	STORE(t0, 0(dst))
	ADDC(sum, t0)
	.set	reorder				/* DADDI_WAR */
	ADD	dst, dst, NBYTES
	bne	rem, len, 1b
	.set	noreorder

	/*
	 * src and dst are aligned, need to copy rem bytes (rem < NBYTES)
	 * A loop would do only a byte at a time with possible branch
	 * mispredicts.	 Can't do an explicit LOAD dst,mask,or,STORE
	 * because can't assume read-access to dst.  Instead, use
	 * STREST dst, which doesn't require read access to dst.
	 *
	 * This code should perform better than a simple loop on modern,
	 * wide-issue mips processors because the code has fewer branches and
	 * more instruction-level parallelism.
	 */
#define bits t2
	beqz	len, .Ldone\@
	 ADD	t1, dst, len	# t1 is just past last byte of dst
	li	bits, 8*NBYTES
	SLL	rem, len, 3	# rem = number of bits to keep
	LOAD(t0, 0(src))
	SUB	bits, bits, rem # bits = number of bits to discard
	SHIFT_DISCARD t0, t0, bits
	STREST(t0, -1(t1))
	SHIFT_DISCARD_REVERT t0, t0, bits
	.set reorder
	ADDC(sum, t0)
	b	.Ldone\@
	.set noreorder
.Ldst_unaligned\@:
	/*
	 * dst is unaligned
	 * t0 = src & ADDRMASK
	 * t1 = dst & ADDRMASK; T1 > 0
	 * len >= NBYTES
	 *
	 * Copy enough bytes to align dst
	 * Set match = (src and dst have same alignment)
	 */
#define match rem
	LDFIRST(t3, FIRST(0)(src))
	ADD	t2, zero, NBYTES
	LDREST(t3, REST(0)(src))
	SUB	t2, t2, t1	# t2 = number of bytes copied
	xor	match, t0, t1
	STFIRST(t3, FIRST(0)(dst))
	SLL	t4, t1, 3		# t4 = number of bits to discard
	SHIFT_DISCARD t3, t3, t4
	/* no SHIFT_DISCARD_REVERT to handle odd buffer properly */
	ADDC(sum, t3)
	beq	len, t2, .Ldone\@
	 SUB	len, len, t2
	ADD	dst, dst, t2
	beqz	match, .Lboth_aligned\@
	 ADD	src, src, t2

.Lsrc_unaligned_dst_aligned\@:
	SRL	t0, len, LOG_NBYTES+2	 # +2 for 4 units/iter
	beqz	t0, .Lcleanup_src_unaligned\@
	 and	rem, len, (4*NBYTES-1)	 # rem = len % 4*NBYTES
1:
/*
 * Avoid consecutive LD*'s to the same register since some mips
 * implementations can't issue them in the same cycle.
 * It's OK to load FIRST(N+1) before REST(N) because the two addresses
 * are to the same unit (unless src is aligned, but it's not).
 */
	LDFIRST(t0, FIRST(0)(src))
	LDFIRST(t1, FIRST(1)(src))
	SUB	len, len, 4*NBYTES
	LDREST(t0, REST(0)(src))
	LDREST(t1, REST(1)(src))
	LDFIRST(t2, FIRST(2)(src))
	LDFIRST(t3, FIRST(3)(src))
	LDREST(t2, REST(2)(src))
	LDREST(t3, REST(3)(src))
	ADD	src, src, 4*NBYTES
#ifdef CONFIG_CPU_SB1
	nop				# improves slotting
#endif
	STORE(t0, UNIT(0)(dst))
	ADDC(t0, t1)
	STORE(t1, UNIT(1)(dst))
	ADDC(sum, t0)
	STORE(t2, UNIT(2)(dst))
	ADDC(t2, t3)
	STORE(t3, UNIT(3)(dst))
	ADDC(sum, t2)
	.set	reorder				/* DADDI_WAR */
	ADD	dst, dst, 4*NBYTES
	bne	len, rem, 1b
	.set	noreorder

.Lcleanup_src_unaligned\@:
	beqz	len, .Ldone\@
	 and	rem, len, NBYTES-1  # rem = len % NBYTES
	beq	rem, len, .Lcopy_bytes\@
	 nop
1:
	LDFIRST(t0, FIRST(0)(src))
	LDREST(t0, REST(0)(src))
	ADD	src, src, NBYTES
	SUB	len, len, NBYTES
	STORE(t0, 0(dst))
	ADDC(sum, t0)
	.set	reorder				/* DADDI_WAR */
	ADD	dst, dst, NBYTES
	bne	len, rem, 1b
	.set	noreorder

.Lcopy_bytes_checklen\@:
	beqz	len, .Ldone\@
	 nop
.Lcopy_bytes\@:
	/* 0 < len < NBYTES  */
#ifdef CONFIG_CPU_LITTLE_ENDIAN
#define SHIFT_START 0
#define SHIFT_INC 8
#else
#define SHIFT_START 8*(NBYTES-1)
#define SHIFT_INC -8
#endif
	move	t2, zero	# partial word
	li	t3, SHIFT_START # shift
#define COPY_BYTE(N)			\
	LOADBU(t0, N(src));		\
	SUB	len, len, 1;		\
	STOREB(t0, N(dst));		\
	SLLV	t0, t0, t3;		\
	addu	t3, SHIFT_INC;		\
	beqz	len, .Lcopy_bytes_done\@; \
	 or	t2, t0

	COPY_BYTE(0)
	COPY_BYTE(1)
#ifdef USE_DOUBLE
	COPY_BYTE(2)
	COPY_BYTE(3)
	COPY_BYTE(4)
	COPY_BYTE(5)
#endif
	LOADBU(t0, NBYTES-2(src))
	SUB	len, len, 1
	STOREB(t0, NBYTES-2(dst))
	SLLV	t0, t0, t3
	or	t2, t0
.Lcopy_bytes_done\@:
	ADDC(sum, t2)
.Ldone\@:
	/* fold checksum */
	.set	push
	.set	noat
#ifdef USE_DOUBLE
	dsll32	v1, sum, 0
	daddu	sum, v1
	sltu	v1, sum, v1
	dsra32	sum, sum, 0
	addu	sum, v1
#endif

#if defined(CONFIG_CPU_MIPSR2) || defined(CONFIG_CPU_MIPSR5) || \
    defined(CONFIG_CPU_LOONGSON64)
	.set	push
	.set	arch=mips32r2
	wsbh	v1, sum
	movn	sum, v1, odd
	.set	pop
#else
	beqz	odd, 1f			/* odd buffer alignment? */
	 lui	v1, 0x00ff
	addu	v1, 0x00ff
	and	t0, sum, v1
	sll	t0, t0, 8
	srl	sum, sum, 8
	and	sum, sum, v1
	or	sum, sum, t0
1:
#endif
	.set	pop
	.set reorder
	jr	ra
	.set noreorder
	.endm

	.set noreorder
.L_exc:
	jr	ra
	 li	v0, 0

FEXPORT(__csum_partial_copy_nocheck)
EXPORT_SYMBOL(__csum_partial_copy_nocheck)
#ifndef CONFIG_EVA
FEXPORT(__csum_partial_copy_to_user)
EXPORT_SYMBOL(__csum_partial_copy_to_user)
FEXPORT(__csum_partial_copy_from_user)
EXPORT_SYMBOL(__csum_partial_copy_from_user)
#endif
__BUILD_CSUM_PARTIAL_COPY_USER LEGACY_MODE USEROP USEROP

#ifdef CONFIG_EVA
LEAF(__csum_partial_copy_to_user)
__BUILD_CSUM_PARTIAL_COPY_USER EVA_MODE KERNELOP USEROP
END(__csum_partial_copy_to_user)

LEAF(__csum_partial_copy_from_user)
__BUILD_CSUM_PARTIAL_COPY_USER EVA_MODE USEROP KERNELOP
END(__csum_partial_copy_from_user)
#endif
