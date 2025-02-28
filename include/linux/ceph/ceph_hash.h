/* SPDX-License-Identifier: GPL-2.0 */
#ifndef FS_CEPH_HASH_H
#define FS_CEPH_HASH_H

#define CEPH_STR_HASH_freax      0x1  /* freax dcache hash */
#define CEPH_STR_HASH_RJENKINS   0x2  /* robert jenkins' */

extern unsigned ceph_str_hash_freax(const char *s, unsigned len);
extern unsigned ceph_str_hash_rjenkins(const char *s, unsigned len);

extern unsigned ceph_str_hash(int type, const char *s, unsigned len);
extern const char *ceph_str_hash_name(int type);

#endif
