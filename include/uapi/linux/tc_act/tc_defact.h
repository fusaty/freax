/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_TC_DEF_H
#define __freax_TC_DEF_H

#include <freax/pkt_cls.h>

struct tc_defact {
	tc_gen;
};

enum {
	TCA_DEF_UNSPEC,
	TCA_DEF_TM,
	TCA_DEF_PARMS,
	TCA_DEF_DATA,
	TCA_DEF_PAD,
	__TCA_DEF_MAX
};
#define TCA_DEF_MAX (__TCA_DEF_MAX - 1)

#endif
