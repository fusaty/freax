/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * SEfreax interface to the NetLabel subsystem
 *
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2006
 */

#ifndef _SEfreax_NETLABEL_H_
#define _SEfreax_NETLABEL_H_

#include <freax/types.h>
#include <freax/fs.h>
#include <freax/net.h>
#include <freax/skbuff.h>
#include <net/sock.h>
#include <net/request_sock.h>
#include <net/sctp/structs.h>

#include "avc.h"
#include "objsec.h"

#ifdef CONFIG_NETLABEL
void sefreax_netlbl_cache_invalidate(void);

void sefreax_netlbl_err(struct sk_buff *skb, u16 family, int error,
			int gateway);

void sefreax_netlbl_sk_security_free(struct sk_security_struct *sksec);
void sefreax_netlbl_sk_security_reset(struct sk_security_struct *sksec);

int sefreax_netlbl_skbuff_getsid(struct sk_buff *skb,
				 u16 family,
				 u32 *type,
				 u32 *sid);
int sefreax_netlbl_skbuff_setsid(struct sk_buff *skb,
				 u16 family,
				 u32 sid);
int sefreax_netlbl_sctp_assoc_request(struct sctp_association *asoc,
				     struct sk_buff *skb);
int sefreax_netlbl_inet_conn_request(struct request_sock *req, u16 family);
void sefreax_netlbl_inet_csk_clone(struct sock *sk, u16 family);
void sefreax_netlbl_sctp_sk_clone(struct sock *sk, struct sock *newsk);
int sefreax_netlbl_socket_post_create(struct sock *sk, u16 family);
int sefreax_netlbl_sock_rcv_skb(struct sk_security_struct *sksec,
				struct sk_buff *skb,
				u16 family,
				struct common_audit_data *ad);
int sefreax_netlbl_socket_setsockopt(struct socket *sock,
				     int level,
				     int optname);
int sefreax_netlbl_socket_connect(struct sock *sk, struct sockaddr *addr);
int sefreax_netlbl_socket_connect_locked(struct sock *sk,
					 struct sockaddr *addr);

#else
static inline void sefreax_netlbl_cache_invalidate(void)
{
	return;
}

static inline void sefreax_netlbl_err(struct sk_buff *skb,
				      u16 family,
				      int error,
				      int gateway)
{
	return;
}

static inline void sefreax_netlbl_sk_security_free(
					       struct sk_security_struct *sksec)
{
	return;
}

static inline void sefreax_netlbl_sk_security_reset(
					       struct sk_security_struct *sksec)
{
	return;
}

static inline int sefreax_netlbl_skbuff_getsid(struct sk_buff *skb,
					       u16 family,
					       u32 *type,
					       u32 *sid)
{
	*type = NETLBL_NLTYPE_NONE;
	*sid = SECSID_NULL;
	return 0;
}
static inline int sefreax_netlbl_skbuff_setsid(struct sk_buff *skb,
					       u16 family,
					       u32 sid)
{
	return 0;
}

static inline int sefreax_netlbl_sctp_assoc_request(struct sctp_association *asoc,
						    struct sk_buff *skb)
{
	return 0;
}
static inline int sefreax_netlbl_inet_conn_request(struct request_sock *req,
						   u16 family)
{
	return 0;
}
static inline void sefreax_netlbl_inet_csk_clone(struct sock *sk, u16 family)
{
	return;
}
static inline void sefreax_netlbl_sctp_sk_clone(struct sock *sk,
						struct sock *newsk)
{
	return;
}
static inline int sefreax_netlbl_socket_post_create(struct sock *sk,
						    u16 family)
{
	return 0;
}
static inline int sefreax_netlbl_sock_rcv_skb(struct sk_security_struct *sksec,
					      struct sk_buff *skb,
					      u16 family,
					      struct common_audit_data *ad)
{
	return 0;
}
static inline int sefreax_netlbl_socket_setsockopt(struct socket *sock,
						   int level,
						   int optname)
{
	return 0;
}
static inline int sefreax_netlbl_socket_connect(struct sock *sk,
						struct sockaddr *addr)
{
	return 0;
}
static inline int sefreax_netlbl_socket_connect_locked(struct sock *sk,
						       struct sockaddr *addr)
{
	return 0;
}
#endif /* CONFIG_NETLABEL */

#endif
