/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_ARRAY_SIZE_H
#define _freax_ARRAY_SIZE_H

#include <freax/compiler.h>

/**
 * ARRAY_SIZE - get the number of elements in array @arr
 * @arr: array to be sized
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#endif  /* _freax_ARRAY_SIZE_H */
