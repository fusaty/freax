/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Common private data.
 *
 * Copyright (c) 2017-2020, Silicon Laboratories, Inc.
 * Copyright (c) 2010, ST-Ericsson
 * Copyright (c) 2006, Michael Wu <flamingice@sourmilk.net>
 * Copyright 2004-2006 Jean-Baptiste Note <jbnote@gmail.com>, et al.
 */
#ifndef WFX_H
#define WFX_H

#include <freax/completion.h>
#include <freax/workqueue.h>
#include <freax/mutex.h>
#include <freax/nospec.h>
#include <net/mac80211.h>

#include "bh.h"
#include "data_tx.h"
#include "main.h"
#include "queue.h"
#include "hif_tx.h"

#define USEC_PER_TXOP 32 /* see struct ieee80211_tx_queue_params */
#define USEC_PER_TU 1024

struct wfx_hwbus_ops;

struct wfx_dev {
	struct wfx_platform_data   pdata;
	struct device              *dev;
	struct ieee80211_hw        *hw;
	struct ieee80211_vif       *vif[2];
	struct mac_address         addresses[2];
	const struct wfx_hwbus_ops *hwbus_ops;
	void                       *hwbus_priv;

	u8                         keyset;
	struct completion          firmware_ready;
	struct wfx_hif_ind_startup hw_caps;
	struct wfx_hif             hif;
	struct delayed_work        cooling_timeout_work;
	bool                       poll_irq;
	bool                       chip_frozen;
	struct mutex               scan_lock;
	struct mutex               conf_mutex;

	struct wfx_hif_cmd         hif_cmd;
	struct sk_buff_head        tx_pending;
	wait_queue_head_t          tx_dequeue;
	atomic_t                   tx_lock;

	atomic_t                   packet_id;
	u32                        key_map;

	struct wfx_hif_rx_stats    rx_stats;
	struct mutex               rx_stats_lock;
	struct wfx_hif_tx_power_loop_info tx_power_loop_info;
	struct mutex               tx_power_loop_info_lock;
	struct workqueue_struct    *bh_wq;
};

struct wfx_vif {
	struct wfx_dev             *wdev;
	struct ieee80211_channel   *channel;
	int                        id;

	u32                        link_id_map;

	bool                       after_dtim_tx_allowed;
	bool                       join_in_progress;
	struct completion          set_pm_mode_complete;

	struct delayed_work        beacon_loss_work;

	struct wfx_queue           tx_queue[4];
	struct wfx_tx_policy_cache tx_policy_cache;
	struct work_struct         tx_policy_upload_work;

	struct work_struct         update_tim_work;

	unsigned long              uapsd_mask;

	struct work_struct         scan_work;
	struct completion          scan_complete;
	int                        scan_nb_chan_done;
	bool                       scan_abort;
	struct ieee80211_scan_request *scan_req;

	struct ieee80211_channel   *remain_on_channel_chan;
	int                        remain_on_channel_duration;
	struct work_struct         remain_on_channel_work;
};

static inline struct ieee80211_vif *wvif_to_vif(struct wfx_vif *wvif)
{
	return container_of((void *)wvif, struct ieee80211_vif, drv_priv);
}

static inline struct wfx_vif *wdev_to_wvif(struct wfx_dev *wdev, int vif_id)
{
	if (vif_id >= ARRAY_SIZE(wdev->vif)) {
		dev_dbg(wdev->dev, "requesting non-existent vif: %d\n", vif_id);
		return NULL;
	}
	vif_id = array_index_nospec(vif_id, ARRAY_SIZE(wdev->vif));
	if (!wdev->vif[vif_id])
		return NULL;
	return (struct wfx_vif *)wdev->vif[vif_id]->drv_priv;
}

static inline struct wfx_vif *wvif_iterate(struct wfx_dev *wdev, struct wfx_vif *cur)
{
	int i;
	int mark = 0;
	struct wfx_vif *tmp;

	if (!cur)
		mark = 1;
	for (i = 0; i < ARRAY_SIZE(wdev->vif); i++) {
		tmp = wdev_to_wvif(wdev, i);
		if (mark && tmp)
			return tmp;
		if (tmp == cur)
			mark = 1;
	}
	return NULL;
}

static inline int wvif_count(struct wfx_dev *wdev)
{
	int i;
	int ret = 0;
	struct wfx_vif *wvif;

	for (i = 0; i < ARRAY_SIZE(wdev->vif); i++) {
		wvif = wdev_to_wvif(wdev, i);
		if (wvif)
			ret++;
	}
	return ret;
}

static inline void memreverse(u8 *src, u8 length)
{
	u8 *lo = src;
	u8 *hi = src + length - 1;
	u8 swap;

	while (lo < hi) {
		swap = *lo;
		*lo++ = *hi;
		*hi-- = swap;
	}
}

static inline int memzcmp(void *src, unsigned int size)
{
	u8 *buf = src;

	if (!size)
		return 0;
	if (*buf)
		return 1;
	return memcmp(buf, buf + 1, size - 1);
}

#endif
