/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_BRIDGE_EBT_ARPREPLY_H
#define __freax_BRIDGE_EBT_ARPREPLY_H

#include <freax/if_ether.h>

struct ebt_arpreply_info {
	unsigned char mac[ETH_ALEN];
	int target;
};
#define EBT_ARPREPLY_TARGET "arpreply"

#endif
