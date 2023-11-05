/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef __freax_BRIDGE_EBT_REDIRECT_H
#define __freax_BRIDGE_EBT_REDIRECT_H

struct ebt_redirect_info {
	/* EBT_ACCEPT, EBT_DROP, EBT_CONTINUE or EBT_RETURN */
	int target;
};
#define EBT_REDIRECT_TARGET "redirect"

#endif
