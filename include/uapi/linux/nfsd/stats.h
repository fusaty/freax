/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
/*
 * freax/include/freax/nfsd/stats.h
 *
 * Statistics for NFS server.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#ifndef _UAPIfreax_NFSD_STATS_H
#define _UAPIfreax_NFSD_STATS_H

#include <freax/nfs4.h>

/* thread usage wraps very million seconds (approx one fortnight) */
#define	NFSD_USAGE_WRAP	(HZ*1000000)

#endif /* _UAPIfreax_NFSD_STATS_H */
