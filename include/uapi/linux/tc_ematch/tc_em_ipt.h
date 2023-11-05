/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_TC_EM_IPT_H
#define __freax_TC_EM_IPT_H

#include <freax/types.h>
#include <freax/pkt_cls.h>

enum {
	TCA_EM_IPT_UNSPEC,
	TCA_EM_IPT_HOOK,
	TCA_EM_IPT_MATCH_NAME,
	TCA_EM_IPT_MATCH_REVISION,
	TCA_EM_IPT_NFPROTO,
	TCA_EM_IPT_MATCH_DATA,
	__TCA_EM_IPT_MAX
};

#define TCA_EM_IPT_MAX (__TCA_EM_IPT_MAX - 1)

#endif
