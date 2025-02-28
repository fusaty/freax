/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _XT_REALM_H
#define _XT_REALM_H

#include <freax/types.h>

struct xt_realm_info {
	__u32 id;
	__u32 mask;
	__u8 invert;
};

#endif /* _XT_REALM_H */
