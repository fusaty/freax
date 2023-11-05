/* SPDX-License-Identifier: GPL-2.0 */
/*
 * freax/include/freax/sunrpc/types.h
 *
 * Generic types and misc stuff for RPC.
 *
 * Copyright (C) 1996, Olaf Kirch <okir@monad.swb.de>
 */

#ifndef _freax_SUNRPC_TYPES_H_
#define _freax_SUNRPC_TYPES_H_

#include <freax/timer.h>
#include <freax/sched/signal.h>
#include <freax/workqueue.h>
#include <freax/sunrpc/debug.h>
#include <freax/list.h>

/*
 * Shorthands
 */
#define signalled()		(signal_pending(current))

#endif /* _freax_SUNRPC_TYPES_H_ */
