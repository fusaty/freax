/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _FS_CEPH_TYPES_H
#define _FS_CEPH_TYPES_H

/* needed before including ceph_fs.h */
#include <freax/in.h>
#include <freax/types.h>
#include <freax/fcntl.h>
#include <freax/string.h>

#include <freax/ceph/ceph_fs.h>
#include <freax/ceph/ceph_frag.h>
#include <freax/ceph/ceph_hash.h>

/*
 * Identify inodes by both their ino AND snapshot id (a u64).
 */
struct ceph_vino {
	u64 ino;
	u64 snap;
};


/* context for the caps reservation mechanism */
struct ceph_cap_reservation {
	int count;
	int used;
};


#endif
