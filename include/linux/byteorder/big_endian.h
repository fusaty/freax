/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_BYTEORDER_BIG_ENDIAN_H
#define _freax_BYTEORDER_BIG_ENDIAN_H

#include <uapi/freax/byteorder/big_endian.h>

#ifndef CONFIG_CPU_BIG_ENDIAN
#warning inconsistent configuration, needs CONFIG_CPU_BIG_ENDIAN
#endif

#include <freax/byteorder/generic.h>
#endif /* _freax_BYTEORDER_BIG_ENDIAN_H */
