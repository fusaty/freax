/* SPDX-License-Identifier: GPL-2.0 */
#ifndef freax_COMPILER_H
#define freax_COMPILER_H

#include "../../../include/freax/compiler_types.h"

#define WRITE_ONCE(var, val) \
	(*((volatile typeof(val) *)(&(var))) = (val))

#define READ_ONCE(var) (*((volatile typeof(var) *)(&(var))))

#define __aligned(x) __attribute((__aligned__(x)))
#endif
