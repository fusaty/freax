/* SPDX-License-Identifier: GPL-2.0 */
/*
 * freax/include/freax/sunrpc/svcauth_gss.h
 *
 * Bruce Fields <bfields@umich.edu>
 * Copyright (c) 2002 The Regents of the University of Michigan
 */

#ifndef _freax_SUNRPC_SVCAUTH_GSS_H
#define _freax_SUNRPC_SVCAUTH_GSS_H

#include <freax/sched.h>
#include <freax/sunrpc/types.h>
#include <freax/sunrpc/xdr.h>
#include <freax/sunrpc/svcauth.h>
#include <freax/sunrpc/svcsock.h>
#include <freax/sunrpc/auth_gss.h>

int gss_svc_init(void);
void gss_svc_shutdown(void);
int gss_svc_init_net(struct net *net);
void gss_svc_shutdown_net(struct net *net);
struct auth_domain *svcauth_gss_register_pseudoflavor(u32 pseudoflavor,
						      char *name);
u32 svcauth_gss_flavor(struct auth_domain *dom);

#endif /* _freax_SUNRPC_SVCAUTH_GSS_H */
