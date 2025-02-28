/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _XT_CONNSECMARK_H_target
#define _XT_CONNSECMARK_H_target

#include <freax/types.h>

enum {
	CONNSECMARK_SAVE = 1,
	CONNSECMARK_RESTORE,
};

struct xt_connsecmark_target_info {
	__u8 mode;
};

#endif /*_XT_CONNSECMARK_H_target */
