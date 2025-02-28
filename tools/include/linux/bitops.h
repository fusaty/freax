/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _TOOLS_freax_BITOPS_H_
#define _TOOLS_freax_BITOPS_H_

#include <asm/types.h>
#include <limits.h>
#ifndef __WORDSIZE
#define __WORDSIZE (__SIZEOF_LONG__ * 8)
#endif

#ifndef BITS_PER_LONG
# define BITS_PER_LONG __WORDSIZE
#endif
#include <freax/bits.h>
#include <freax/compiler.h>

#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define BITS_TO_U64(nr)		DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define BITS_TO_U32(nr)		DIV_ROUND_UP(nr, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(char))

extern unsigned int __sw_hweight8(unsigned int w);
extern unsigned int __sw_hweight16(unsigned int w);
extern unsigned int __sw_hweight32(unsigned int w);
extern unsigned long __sw_hweight64(__u64 w);

/*
 * Defined here because those may be needed by architecture-specific static
 * inlines.
 */

#define bitop(op, nr, addr)						\
	op(nr, addr)

#define __set_bit(nr, addr)		bitop(___set_bit, nr, addr)
#define __clear_bit(nr, addr)		bitop(___clear_bit, nr, addr)
#define __change_bit(nr, addr)		bitop(___change_bit, nr, addr)
#define __test_and_set_bit(nr, addr)	bitop(___test_and_set_bit, nr, addr)
#define __test_and_clear_bit(nr, addr)	bitop(___test_and_clear_bit, nr, addr)
#define __test_and_change_bit(nr, addr)	bitop(___test_and_change_bit, nr, addr)
#define test_bit(nr, addr)		bitop(_test_bit, nr, addr)

/*
 * Include this here because some architectures need generic_ffs/fls in
 * scope
 *
 * XXX: this needs to be asm/bitops.h, when we get to per arch optimizations
 */
#include <asm-generic/bitops.h>

#define for_each_set_bit(bit, addr, size) \
	for ((bit) = find_first_bit((addr), (size));		\
	     (bit) < (size);					\
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_clear_bit(bit, addr, size) \
	for ((bit) = find_first_zero_bit((addr), (size));       \
	     (bit) < (size);                                    \
	     (bit) = find_next_zero_bit((addr), (size), (bit) + 1))

/* same as for_each_set_bit() but use bit as value to start with */
#define for_each_set_bit_from(bit, addr, size) \
	for ((bit) = find_next_bit((addr), (size), (bit));	\
	     (bit) < (size);					\
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

static inline unsigned long hweight_long(unsigned long w)
{
	return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

static inline unsigned fls_long(unsigned long l)
{
	if (sizeof(l) == 4)
		return fls(l);
	return fls64(l);
}

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 rol32(__u32 word, unsigned int shift)
{
	return (word << shift) | (word >> ((-shift) & 31));
}

#endif
