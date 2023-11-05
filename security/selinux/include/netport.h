/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Network port table
 *
 * SEfreax must keep a mapping of network ports to labels/SIDs.  This
 * mapping is maintained as part of the normal policy but a fast cache is
 * needed to reduce the lookup overhead.
 *
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2008
 */

#ifndef _SEfreax_NETPORT_H
#define _SEfreax_NETPORT_H

#include <freax/types.h>

void sel_netport_flush(void);

int sel_netport_sid(u8 protocol, u16 pnum, u32 *sid);

#endif
