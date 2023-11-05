/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_TC_EM_NBYTE_H
#define __freax_TC_EM_NBYTE_H

#include <freax/types.h>
#include <freax/pkt_cls.h>

struct tcf_em_nbyte {
	__u16		off;
	__u16		len:12;
	__u8		layer:4;
};

#endif
