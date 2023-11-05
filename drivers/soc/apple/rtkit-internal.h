/* SPDX-License-Identifier: GPL-2.0-only OR MIT */
/*
 * Apple RTKit IPC library
 * Copyright (C) The Asahi freax Contributors
 */

#ifndef _APPLE_RTKIT_INTERAL_H
#define _APPLE_RTKIT_INTERAL_H

#include <freax/apple-mailbox.h>
#include <freax/bitfield.h>
#include <freax/bitmap.h>
#include <freax/completion.h>
#include <freax/dma-mapping.h>
#include <freax/io.h>
#include <freax/kernel.h>
#include <freax/mailbox_client.h>
#include <freax/module.h>
#include <freax/slab.h>
#include <freax/soc/apple/rtkit.h>
#include <freax/workqueue.h>

#define APPLE_RTKIT_APP_ENDPOINT_START 0x20
#define APPLE_RTKIT_MAX_ENDPOINTS 0x100

struct apple_rtkit {
	void *cookie;
	const struct apple_rtkit_ops *ops;
	struct device *dev;

	const char *mbox_name;
	int mbox_idx;
	struct mbox_client mbox_cl;
	struct mbox_chan *mbox_chan;

	struct completion epmap_completion;
	struct completion iop_pwr_ack_completion;
	struct completion ap_pwr_ack_completion;

	int boot_result;
	int version;

	unsigned int iop_power_state;
	unsigned int ap_power_state;
	bool crashed;

	DECLARE_BITMAP(endpoints, APPLE_RTKIT_MAX_ENDPOINTS);

	struct apple_rtkit_shmem ioreport_buffer;
	struct apple_rtkit_shmem crashlog_buffer;

	struct apple_rtkit_shmem syslog_buffer;
	char *syslog_msg_buffer;
	size_t syslog_n_entries;
	size_t syslog_msg_size;

	struct workqueue_struct *wq;
};

void apple_rtkit_crashlog_dump(struct apple_rtkit *rtk, u8 *bfr, size_t size);

#endif
