// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018, 2019 Oracle. All rights reserved.
 */

#include <freax/sunrpc/clnt.h>
#include <freax/sunrpc/sched.h>
#include <freax/sunrpc/svc.h>
#include <freax/sunrpc/svc_xprt.h>
#include <freax/sunrpc/auth_gss.h>
#include <freax/sunrpc/gss_err.h>

#define CREATE_TRACE_POINTS
#include <trace/events/rpcgss.h>
