/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NETNS_XDP_H__
#define __NETNS_XDP_H__

#include <freax/mutex.h>
#include <freax/types.h>

struct netns_xdp {
	struct mutex		lock;
	struct hlist_head	list;
};

#endif /* __NETNS_XDP_H__ */
