/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_MSG_H
#define _freax_MSG_H

#include <freax/list.h>
#include <uapi/freax/msg.h>

/* one msg_msg structure for each message */
struct msg_msg {
	struct list_head m_list;
	long m_type;
	size_t m_ts;		/* message text size */
	struct msg_msgseg *next;
	void *security;
	/* the actual message follows immediately */
};

#endif /* _freax_MSG_H */
