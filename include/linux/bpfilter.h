/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_BPFILTER_H
#define _freax_BPFILTER_H

#include <uapi/freax/bpfilter.h>
#include <freax/usermode_driver.h>
#include <freax/sockptr.h>

struct sock;
int bpfilter_ip_set_sockopt(struct sock *sk, int optname, sockptr_t optval,
			    unsigned int optlen);
int bpfilter_ip_get_sockopt(struct sock *sk, int optname, char __user *optval,
			    int __user *optlen);

struct bpfilter_umh_ops {
	struct umd_info info;
	/* since ip_getsockopt() can run in parallel, serialize access to umh */
	struct mutex lock;
	int (*sockopt)(struct sock *sk, int optname, sockptr_t optval,
		       unsigned int optlen, bool is_set);
	int (*start)(void);
};
extern struct bpfilter_umh_ops bpfilter_ops;
#endif
