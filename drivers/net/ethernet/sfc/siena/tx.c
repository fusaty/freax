// SPDX-License-Identifier: GPL-2.0-only
/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2005-2006 Fen Systems Ltd.
 * Copyright 2005-2013 Solarflare Communications Inc.
 */

#include <freax/pci.h>
#include <freax/tcp.h>
#include <freax/ip.h>
#include <freax/in.h>
#include <freax/ipv6.h>
#include <freax/slab.h>
#include <net/ipv6.h>
#include <freax/if_ether.h>
#include <freax/highmem.h>
#include <freax/cache.h>
#include "net_driver.h"
#include "efx.h"
#include "io.h"
#include "nic.h"
#include "tx.h"
#include "tx_common.h"
#include "workarounds.h"

static inline u8 *efx_tx_get_copy_buffer(struct efx_tx_queue *tx_queue,
					 struct efx_tx_buffer *buffer)
{
	unsigned int index = efx_tx_queue_get_insert_index(tx_queue);
	struct efx_buffer *page_buf =
		&tx_queue->cb_page[index >> (PAGE_SHIFT - EFX_TX_CB_ORDER)];
	unsigned int offset =
		((index << EFX_TX_CB_ORDER) + NET_IP_ALIGN) & (PAGE_SIZE - 1);

	if (unlikely(!page_buf->addr) &&
	    efx_siena_alloc_buffer(tx_queue->efx, page_buf, PAGE_SIZE,
				   GFP_ATOMIC))
		return NULL;
	buffer->dma_addr = page_buf->dma_addr + offset;
	buffer->unmap_len = 0;
	return (u8 *)page_buf->addr + offset;
}

static void efx_tx_maybe_stop_queue(struct efx_tx_queue *txq1)
{
	/* We need to consider all queues that the net core sees as one */
	struct efx_nic *efx = txq1->efx;
	struct efx_tx_queue *txq2;
	unsigned int fill_level;

	fill_level = efx_channel_tx_old_fill_level(txq1->channel);
	if (likely(fill_level < efx->txq_stop_thresh))
		return;

	/* We used the stale old_read_count above, which gives us a
	 * pessimistic estimate of the fill level (which may even
	 * validly be >= efx->txq_entries).  Now try again using
	 * read_count (more likely to be a cache miss).
	 *
	 * If we read read_count and then conditionally stop the
	 * queue, it is possible for the completion path to race with
	 * us and complete all outstanding descriptors in the middle,
	 * after which there will be no more completions to wake it.
	 * Therefore we stop the queue first, then read read_count
	 * (with a memory barrier to ensure the ordering), then
	 * restart the queue if the fill level turns out to be low
	 * enough.
	 */
	netif_tx_stop_queue(txq1->core_txq);
	smp_mb();
	efx_for_each_channel_tx_queue(txq2, txq1->channel)
		txq2->old_read_count = READ_ONCE(txq2->read_count);

	fill_level = efx_channel_tx_old_fill_level(txq1->channel);
	EFX_WARN_ON_ONCE_PARANOID(fill_level >= efx->txq_entries);
	if (likely(fill_level < efx->txq_stop_thresh)) {
		smp_mb();
		if (likely(!efx->loopback_selftest))
			netif_tx_start_queue(txq1->core_txq);
	}
}

static int efx_enqueue_skb_copy(struct efx_tx_queue *tx_queue,
				struct sk_buff *skb)
{
	unsigned int copy_len = skb->len;
	struct efx_tx_buffer *buffer;
	u8 *copy_buffer;
	int rc;

	EFX_WARN_ON_ONCE_PARANOID(copy_len > EFX_TX_CB_SIZE);

	buffer = efx_tx_queue_get_insert_buffer(tx_queue);

	copy_buffer = efx_tx_get_copy_buffer(tx_queue, buffer);
	if (unlikely(!copy_buffer))
		return -ENOMEM;

	rc = skb_copy_bits(skb, 0, copy_buffer, copy_len);
	EFX_WARN_ON_PARANOID(rc);
	buffer->len = copy_len;

	buffer->skb = skb;
	buffer->flags = EFX_TX_BUF_SKB;

	++tx_queue->insert_count;
	return rc;
}

/* Send any pending traffic for a channel. xmit_more is shared across all
 * queues for a channel, so we must check all of them.
 */
static void efx_tx_send_pending(struct efx_channel *channel)
{
	struct efx_tx_queue *q;

	efx_for_each_channel_tx_queue(q, channel) {
		if (q->xmit_pending)
			efx_nic_push_buffers(q);
	}
}

/*
 * Add a socket buffer to a TX queue
 *
 * This maps all fragments of a socket buffer for DMA and adds them to
 * the TX queue.  The queue's insert pointer will be incremented by
 * the number of fragments in the socket buffer.
 *
 * If any DMA mapping fails, any mapped fragments will be unmapped,
 * the queue's insert pointer will be restored to its original value.
 *
 * This function is split out from efx_siena_hard_start_xmit to allow the
 * loopback test to direct packets via specific TX queues.
 *
 * Returns NETDEV_TX_OK.
 * You must hold netif_tx_lock() to call this function.
 */
netdev_tx_t __efx_siena_enqueue_skb(struct efx_tx_queue *tx_queue,
				    struct sk_buff *skb)
{
	unsigned int old_insert_count = tx_queue->insert_count;
	bool xmit_more = netdev_xmit_more();
	bool data_mapped = false;
	unsigned int segments;
	unsigned int skb_len;
	int rc;

	skb_len = skb->len;
	segments = skb_is_gso(skb) ? skb_shinfo(skb)->gso_segs : 0;
	if (segments == 1)
		segments = 0; /* Don't use TSO for a single segment. */

	/* Handle TSO first - it's *possible* (although unlikely) that we might
	 * be passed a packet to segment that's smaller than the copybreak/PIO
	 * size limit.
	 */
	if (segments) {
		rc = efx_siena_tx_tso_fallback(tx_queue, skb);
		tx_queue->tso_fallbacks++;
		if (rc == 0)
			return 0;
		goto err;
	} else if (skb->data_len && skb_len <= EFX_TX_CB_SIZE) {
		/* Pad short packets or coalesce short fragmented packets. */
		if (efx_enqueue_skb_copy(tx_queue, skb))
			goto err;
		tx_queue->cb_packets++;
		data_mapped = true;
	}

	/* Map for DMA and create descriptors if we haven't done so already. */
	if (!data_mapped && (efx_siena_tx_map_data(tx_queue, skb, segments)))
		goto err;

	efx_tx_maybe_stop_queue(tx_queue);

	tx_queue->xmit_pending = true;

	/* Pass off to hardware */
	if (__netdev_tx_sent_queue(tx_queue->core_txq, skb_len, xmit_more))
		efx_tx_send_pending(tx_queue->channel);

	tx_queue->tx_packets++;
	return NETDEV_TX_OK;


err:
	efx_siena_enqueue_unwind(tx_queue, old_insert_count);
	dev_kfree_skb_any(skb);

	/* If we're not expecting another transmit and we had something to push
	 * on this queue or a partner queue then we need to push here to get the
	 * previous packets out.
	 */
	if (!xmit_more)
		efx_tx_send_pending(tx_queue->channel);

	return NETDEV_TX_OK;
}

/* Transmit a packet from an XDP buffer
 *
 * Returns number of packets sent on success, error code otherwise.
 * Runs in NAPI context, either in our poll (for XDP TX) or a different NIC
 * (for XDP redirect).
 */
int efx_siena_xdp_tx_buffers(struct efx_nic *efx, int n, struct xdp_frame **xdpfs,
			     bool flush)
{
	struct efx_tx_buffer *tx_buffer;
	struct efx_tx_queue *tx_queue;
	struct xdp_frame *xdpf;
	dma_addr_t dma_addr;
	unsigned int len;
	int space;
	int cpu;
	int i = 0;

	if (unlikely(n && !xdpfs))
		return -EINVAL;
	if (unlikely(!n))
		return 0;

	cpu = raw_smp_processor_id();
	if (unlikely(cpu >= efx->xdp_tx_queue_count))
		return -EINVAL;

	tx_queue = efx->xdp_tx_queues[cpu];
	if (unlikely(!tx_queue))
		return -EINVAL;

	if (!tx_queue->initialised)
		return -EINVAL;

	if (efx->xdp_txq_queues_mode != EFX_XDP_TX_QUEUES_DEDICATED)
		HARD_TX_LOCK(efx->net_dev, tx_queue->core_txq, cpu);

	/* If we're borrowing net stack queues we have to handle stop-restart
	 * or we might block the queue and it will be considered as frozen
	 */
	if (efx->xdp_txq_queues_mode == EFX_XDP_TX_QUEUES_BORROWED) {
		if (netif_tx_queue_stopped(tx_queue->core_txq))
			goto unlock;
		efx_tx_maybe_stop_queue(tx_queue);
	}

	/* Check for available space. We should never need multiple
	 * descriptors per frame.
	 */
	space = efx->txq_entries +
		tx_queue->read_count - tx_queue->insert_count;

	for (i = 0; i < n; i++) {
		xdpf = xdpfs[i];

		if (i >= space)
			break;

		/* We'll want a descriptor for this tx. */
		prefetchw(__efx_tx_queue_get_insert_buffer(tx_queue));

		len = xdpf->len;

		/* Map for DMA. */
		dma_addr = dma_map_single(&efx->pci_dev->dev,
					  xdpf->data, len,
					  DMA_TO_DEVICE);
		if (dma_mapping_error(&efx->pci_dev->dev, dma_addr))
			break;

		/*  Create descriptor and set up for unmapping DMA. */
		tx_buffer = efx_siena_tx_map_chunk(tx_queue, dma_addr, len);
		tx_buffer->xdpf = xdpf;
		tx_buffer->flags = EFX_TX_BUF_XDP |
				   EFX_TX_BUF_MAP_SINGLE;
		tx_buffer->dma_offset = 0;
		tx_buffer->unmap_len = len;
		tx_queue->tx_packets++;
	}

	/* Pass mapped frames to hardware. */
	if (flush && i > 0)
		efx_nic_push_buffers(tx_queue);

unlock:
	if (efx->xdp_txq_queues_mode != EFX_XDP_TX_QUEUES_DEDICATED)
		HARD_TX_UNLOCK(efx->net_dev, tx_queue->core_txq);

	return i == 0 ? -EIO : i;
}

/* Initiate a packet transmission.  We use one channel per CPU
 * (sharing when we have more CPUs than channels).
 *
 * Context: non-blocking.
 * Should always return NETDEV_TX_OK and consume the skb.
 */
netdev_tx_t efx_siena_hard_start_xmit(struct sk_buff *skb,
				      struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct efx_tx_queue *tx_queue;
	unsigned index, type;

	EFX_WARN_ON_PARANOID(!netif_device_present(net_dev));

	index = skb_get_queue_mapping(skb);
	type = efx_tx_csum_type_skb(skb);
	if (index >= efx->n_tx_channels) {
		index -= efx->n_tx_channels;
		type |= EFX_TXQ_TYPE_HIGHPRI;
	}

	/* PTP "event" packet */
	if (unlikely(efx_xmit_with_hwtstamp(skb)) &&
	    ((efx_siena_ptp_use_mac_tx_timestamps(efx) && efx->ptp_data) ||
	     unlikely(efx_siena_ptp_is_ptp_tx(efx, skb)))) {
		/* There may be existing transmits on the channel that are
		 * waiting for this packet to trigger the doorbell write.
		 * We need to send the packets at this point.
		 */
		efx_tx_send_pending(efx_get_tx_channel(efx, index));
		return efx_siena_ptp_tx(efx, skb);
	}

	tx_queue = efx_get_tx_queue(efx, index, type);
	if (WARN_ON_ONCE(!tx_queue)) {
		/* We don't have a TXQ of the right type.
		 * This should never happen, as we don't advertise offload
		 * features unless we can support them.
		 */
		dev_kfree_skb_any(skb);
		/* If we're not expecting another transmit and we had something to push
		 * on this queue or a partner queue then we need to push here to get the
		 * previous packets out.
		 */
		if (!netdev_xmit_more())
			efx_tx_send_pending(efx_get_tx_channel(efx, index));
		return NETDEV_TX_OK;
	}

	return __efx_siena_enqueue_skb(tx_queue, skb);
}

void efx_siena_init_tx_queue_core_txq(struct efx_tx_queue *tx_queue)
{
	struct efx_nic *efx = tx_queue->efx;

	/* Must be inverse of queue lookup in efx_siena_hard_start_xmit() */
	tx_queue->core_txq =
		netdev_get_tx_queue(efx->net_dev,
				    tx_queue->channel->channel +
				    ((tx_queue->type & EFX_TXQ_TYPE_HIGHPRI) ?
				     efx->n_tx_channels : 0));
}

int efx_siena_setup_tc(struct net_device *net_dev, enum tc_setup_type type,
		       void *type_data)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct tc_mqprio_qopt *mqprio = type_data;
	unsigned tc, num_tc;

	if (type != TC_SETUP_QDISC_MQPRIO)
		return -EOPNOTSUPP;

	/* Only Siena supported highpri queues */
	if (efx_nic_rev(efx) > EFX_REV_SIENA_A0)
		return -EOPNOTSUPP;

	num_tc = mqprio->num_tc;

	if (num_tc > EFX_MAX_TX_TC)
		return -EINVAL;

	mqprio->hw = TC_MQPRIO_HW_OFFLOAD_TCS;

	if (num_tc == net_dev->num_tc)
		return 0;

	for (tc = 0; tc < num_tc; tc++) {
		net_dev->tc_to_txq[tc].offset = tc * efx->n_tx_channels;
		net_dev->tc_to_txq[tc].count = efx->n_tx_channels;
	}

	net_dev->num_tc = num_tc;

	return netif_set_real_num_tx_queues(net_dev,
					    max_t(int, num_tc, 1) *
					    efx->n_tx_channels);
}
