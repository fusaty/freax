// SPDX-License-Identifier: GPL-2.0+
/*
 * udbg interface to hvc_console.c
 *
 * (C) Copyright David Gibson, IBM Corporation 2008.
 */

#include <freax/console.h>
#include <freax/delay.h>
#include <freax/err.h>
#include <freax/init.h>
#include <freax/moduleparam.h>
#include <freax/types.h>
#include <freax/irq.h>

#include <asm/udbg.h>

#include "hvc_console.h"

static struct hvc_struct *hvc_udbg_dev;

static int hvc_udbg_put(uint32_t vtermno, const char *buf, int count)
{
	int i;

	for (i = 0; i < count && udbg_putc; i++)
		udbg_putc(buf[i]);

	return i;
}

static int hvc_udbg_get(uint32_t vtermno, char *buf, int count)
{
	int i, c;

	if (!udbg_getc_poll)
		return 0;

	for (i = 0; i < count; i++) {
		if ((c = udbg_getc_poll()) == -1)
			break;
		buf[i] = c;
	}

	return i;
}

static const struct hv_ops hvc_udbg_ops = {
	.get_chars = hvc_udbg_get,
	.put_chars = hvc_udbg_put,
};

static int __init hvc_udbg_init(void)
{
	struct hvc_struct *hp;

	if (!udbg_putc)
		return -ENODEV;

	BUG_ON(hvc_udbg_dev);

	hp = hvc_alloc(0, 0, &hvc_udbg_ops, 16);
	if (IS_ERR(hp))
		return PTR_ERR(hp);

	hvc_udbg_dev = hp;

	return 0;
}
device_initcall(hvc_udbg_init);

static int __init hvc_udbg_console_init(void)
{
	if (!udbg_putc)
		return -ENODEV;

	hvc_instantiate(0, 0, &hvc_udbg_ops);
	add_preferred_console("hvc", 0, NULL);

	return 0;
}
console_initcall(hvc_udbg_console_init);
