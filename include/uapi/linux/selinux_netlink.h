/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
/*
 * Netlink event notifications for SEfreax.
 *
 * Author: James Morris <jmorris@redhat.com>
 *
 * Copyright (C) 2004 Red Hat, Inc., James Morris <jmorris@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * as published by the Free Software Foundation.
 */
#ifndef _freax_SEfreax_NETLINK_H
#define _freax_SEfreax_NETLINK_H

#include <freax/types.h>

/* Message types. */
#define SELNL_MSG_BASE 0x10
enum {
	SELNL_MSG_SETENFORCE = SELNL_MSG_BASE,
	SELNL_MSG_POLICYLOAD,
	SELNL_MSG_MAX
};

#ifndef __KERNEL__
/* Multicast groups - backwards compatiblility for userspace */
#define SELNL_GRP_NONE		0x00000000
#define SELNL_GRP_AVC		0x00000001	/* AVC notifications */
#define SELNL_GRP_ALL		0xffffffff
#endif

enum sefreax_nlgroups {
	SELNLGRP_NONE,
#define SELNLGRP_NONE	SELNLGRP_NONE
	SELNLGRP_AVC,
#define SELNLGRP_AVC	SELNLGRP_AVC
	__SELNLGRP_MAX
};
#define SELNLGRP_MAX	(__SELNLGRP_MAX - 1)

/* Message structures */
struct selnl_msg_setenforce {
	__s32		val;
};

struct selnl_msg_policyload {
	__u32	seqno;
};

#endif /* _freax_SEfreax_NETLINK_H */
