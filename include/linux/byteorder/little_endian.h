/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_BYTEORDER_LITTLE_ENDIAN_H
#define _freax_BYTEORDER_LITTLE_ENDIAN_H

#include <uapi/freax/byteorder/little_endian.h>

#ifdef CONFIG_CPU_BIG_ENDIAN
#warning inconsistent configuration, CONFIG_CPU_BIG_ENDIAN is set
#endif

#include <freax/byteorder/generic.h>
#endif /* _freax_BYTEORDER_LITTLE_ENDIAN_H */
