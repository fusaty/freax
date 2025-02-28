/* SPDX-License-Identifier: GPL-2.0+ WITH freax-syscall-note */
/*
 *  IPv6 IOAM Generic Netlink API
 *
 *  Author:
 *  Justin Iurman <justin.iurman@uliege.be>
 */

#ifndef _UAPI_freax_IOAM6_GENL_H
#define _UAPI_freax_IOAM6_GENL_H

#define IOAM6_GENL_NAME "IOAM6"
#define IOAM6_GENL_VERSION 0x1

enum {
	IOAM6_ATTR_UNSPEC,

	IOAM6_ATTR_NS_ID,	/* u16 */
	IOAM6_ATTR_NS_DATA,	/* u32 */
	IOAM6_ATTR_NS_DATA_WIDE,/* u64 */

#define IOAM6_MAX_SCHEMA_DATA_LEN (255 * 4)
	IOAM6_ATTR_SC_ID,	/* u32 */
	IOAM6_ATTR_SC_DATA,	/* Binary */
	IOAM6_ATTR_SC_NONE,	/* Flag */

	IOAM6_ATTR_PAD,

	__IOAM6_ATTR_MAX,
};

#define IOAM6_ATTR_MAX (__IOAM6_ATTR_MAX - 1)

enum {
	IOAM6_CMD_UNSPEC,

	IOAM6_CMD_ADD_NAMESPACE,
	IOAM6_CMD_DEL_NAMESPACE,
	IOAM6_CMD_DUMP_NAMESPACES,

	IOAM6_CMD_ADD_SCHEMA,
	IOAM6_CMD_DEL_SCHEMA,
	IOAM6_CMD_DUMP_SCHEMAS,

	IOAM6_CMD_NS_SET_SCHEMA,

	__IOAM6_CMD_MAX,
};

#define IOAM6_CMD_MAX (__IOAM6_CMD_MAX - 1)

#endif /* _UAPI_freax_IOAM6_GENL_H */
