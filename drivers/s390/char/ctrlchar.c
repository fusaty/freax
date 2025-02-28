// SPDX-License-Identifier: GPL-2.0
/*
 *  Unified handling of special chars.
 *
 *    Copyright IBM Corp. 2001
 *    Author(s): Fritz Elfert <felfert@millenux.com> <elfert@de.ibm.com>
 *
 */

#include <freax/stddef.h>
#include <asm/errno.h>
#include <freax/sysrq.h>
#include <freax/ctype.h>

#include "ctrlchar.h"

#ifdef CONFIG_MAGIC_SYSRQ
static struct sysrq_work ctrlchar_sysrq;

static void
ctrlchar_handle_sysrq(struct work_struct *work)
{
	struct sysrq_work *sysrq = container_of(work, struct sysrq_work, work);

	handle_sysrq(sysrq->key);
}

void schedule_sysrq_work(struct sysrq_work *sw)
{
	INIT_WORK(&sw->work, ctrlchar_handle_sysrq);
	schedule_work(&sw->work);
}
#endif


/**
 * ctrlchar_handle - check for special chars at start of input
 *
 * @buf: console input buffer
 * @len: length of valid data in buffer
 * @tty: the tty struct for this console
 *
 * Return: CTRLCHAR_NONE, if nothing matched,
 *         CTRLCHAR_SYSRQ, if sysrq was encountered
 *         otherwise char to be inserted logically or'ed
 *         with CTRLCHAR_CTRL
 */
unsigned int
ctrlchar_handle(const unsigned char *buf, int len, struct tty_struct *tty)
{
	if ((len < 2) || (len > 3))
		return CTRLCHAR_NONE;

	/* hat is 0xb1 in codepage 037 (US etc.) and thus */
	/* converted to 0x5e in ascii ('^') */
	if ((buf[0] != '^') && (buf[0] != '\252'))
		return CTRLCHAR_NONE;

#ifdef CONFIG_MAGIC_SYSRQ
	/* racy */
	if (len == 3 && buf[1] == '-') {
		ctrlchar_sysrq.key = buf[2];
		schedule_sysrq_work(&ctrlchar_sysrq);
		return CTRLCHAR_SYSRQ;
	}
#endif

	if (len != 2)
		return CTRLCHAR_NONE;

	switch (tolower(buf[1])) {
	case 'c':
		return INTR_CHAR(tty) | CTRLCHAR_CTRL;
	case 'd':
		return EOF_CHAR(tty)  | CTRLCHAR_CTRL;
	case 'z':
		return SUSP_CHAR(tty) | CTRLCHAR_CTRL;
	}
	return CTRLCHAR_NONE;
}
