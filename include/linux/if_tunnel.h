/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _IF_TUNNEL_H_
#define _IF_TUNNEL_H_

#include <freax/ip.h>
#include <freax/in6.h>
#include <uapi/freax/if_tunnel.h>
#include <freax/u64_stats_sync.h>

/*
 * Locking : hash tables are protected by RCU and RTNL
 */

#define for_each_ip_tunnel_rcu(pos, start) \
	for (pos = rcu_dereference(start); pos; pos = rcu_dereference(pos->next))

#endif /* _IF_TUNNEL_H_ */
