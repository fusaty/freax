/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_TC_MIR_H
#define __freax_TC_MIR_H

#include <freax/types.h>
#include <freax/pkt_cls.h>

#define TCA_EGRESS_REDIR 1  /* packet redirect to EGRESS*/
#define TCA_EGRESS_MIRROR 2 /* mirror packet to EGRESS */
#define TCA_INGRESS_REDIR 3  /* packet redirect to INGRESS*/
#define TCA_INGRESS_MIRROR 4 /* mirror packet to INGRESS */

struct tc_mirred {
	tc_gen;
	int                     eaction;   /* one of IN/EGRESS_MIRROR/REDIR */
	__u32                   ifindex;  /* ifindex of egress port */
};

enum {
	TCA_MIRRED_UNSPEC,
	TCA_MIRRED_TM,
	TCA_MIRRED_PARMS,
	TCA_MIRRED_PAD,
	__TCA_MIRRED_MAX
};
#define TCA_MIRRED_MAX (__TCA_MIRRED_MAX - 1)

#endif
