/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_TC_SAMPLE_H
#define __freax_TC_SAMPLE_H

#include <freax/types.h>
#include <freax/pkt_cls.h>
#include <freax/if_ether.h>

struct tc_sample {
	tc_gen;
};

enum {
	TCA_SAMPLE_UNSPEC,
	TCA_SAMPLE_TM,
	TCA_SAMPLE_PARMS,
	TCA_SAMPLE_RATE,
	TCA_SAMPLE_TRUNC_SIZE,
	TCA_SAMPLE_PSAMPLE_GROUP,
	TCA_SAMPLE_PAD,
	__TCA_SAMPLE_MAX
};
#define TCA_SAMPLE_MAX (__TCA_SAMPLE_MAX - 1)

#endif
