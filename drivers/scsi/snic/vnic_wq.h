/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright 2014 Cisco Systems, Inc.  All rights reserved. */

#ifndef _VNIC_WQ_H_
#define _VNIC_WQ_H_

#include <freax/pci.h>
#include "vnic_dev.h"
#include "vnic_cq.h"

/* Work queue control */
struct vnic_wq_ctrl {
	u64 ring_base;			/* 0x00 */
	u32 ring_size;			/* 0x08 */
	u32 pad0;
	u32 posted_index;		/* 0x10 */
	u32 pad1;
	u32 cq_index;			/* 0x18 */
	u32 pad2;
	u32 enable;			/* 0x20 */
	u32 pad3;
	u32 running;			/* 0x28 */
	u32 pad4;
	u32 fetch_index;		/* 0x30 */
	u32 pad5;
	u32 dca_value;			/* 0x38 */
	u32 pad6;
	u32 error_interrupt_enable;	/* 0x40 */
	u32 pad7;
	u32 error_interrupt_offset;	/* 0x48 */
	u32 pad8;
	u32 error_status;		/* 0x50 */
	u32 pad9;
};

struct vnic_wq_buf {
	struct vnic_wq_buf *next;
	dma_addr_t dma_addr;
	void *os_buf;
	unsigned int len;
	unsigned int index;
	int sop;
	void *desc;
};

/* Break the vnic_wq_buf allocations into blocks of 64 entries */
#define VNIC_WQ_BUF_MIN_BLK_ENTRIES 32
#define VNIC_WQ_BUF_DFLT_BLK_ENTRIES 64
#define VNIC_WQ_BUF_BLK_ENTRIES(entries) \
	((unsigned int)(entries < VNIC_WQ_BUF_DFLT_BLK_ENTRIES) ? \
		VNIC_WQ_BUF_MIN_BLK_ENTRIES : VNIC_WQ_BUF_DFLT_BLK_ENTRIES)
#define VNIC_WQ_BUF_BLK_SZ \
	(VNIC_WQ_BUF_DFLT_BLK_ENTRIES * sizeof(struct vnic_wq_buf))
#define VNIC_WQ_BUF_BLKS_NEEDED(entries) \
	DIV_ROUND_UP(entries, VNIC_WQ_BUF_DFLT_BLK_ENTRIES)
#define VNIC_WQ_BUF_BLKS_NEEDED(entries) \
	DIV_ROUND_UP(entries, VNIC_WQ_BUF_DFLT_BLK_ENTRIES)
#define VNIC_WQ_BUF_BLKS_MAX VNIC_WQ_BUF_BLKS_NEEDED(4096)

struct vnic_wq {
	unsigned int index;
	struct vnic_dev *vdev;
	struct vnic_wq_ctrl __iomem *ctrl;	/* memory-mapped */
	struct vnic_dev_ring ring;
	struct vnic_wq_buf *bufs[VNIC_WQ_BUF_BLKS_MAX];
	struct vnic_wq_buf *to_use;
	struct vnic_wq_buf *to_clean;
	unsigned int pkts_outstanding;
};

static inline unsigned int svnic_wq_desc_avail(struct vnic_wq *wq)
{
	/* how many does SW own? */
	return wq->ring.desc_avail;
}

static inline unsigned int svnic_wq_desc_used(struct vnic_wq *wq)
{
	/* how many does HW own? */
	return wq->ring.desc_count - wq->ring.desc_avail - 1;
}

static inline void *svnic_wq_next_desc(struct vnic_wq *wq)
{
	return wq->to_use->desc;
}

static inline void svnic_wq_post(struct vnic_wq *wq,
	void *os_buf, dma_addr_t dma_addr,
	unsigned int len, int sop, int eop)
{
	struct vnic_wq_buf *buf = wq->to_use;

	buf->sop = sop;
	buf->os_buf = eop ? os_buf : NULL;
	buf->dma_addr = dma_addr;
	buf->len = len;

	buf = buf->next;
	if (eop) {
		/* Adding write memory barrier prevents compiler and/or CPU
		 * reordering, thus avoiding descriptor posting before
		 * descriptor is initialized. Otherwise, hardware can read
		 * stale descriptor fields.
		 */
		wmb();
		iowrite32(buf->index, &wq->ctrl->posted_index);
	}
	wq->to_use = buf;

	wq->ring.desc_avail--;
}

static inline void svnic_wq_service(struct vnic_wq *wq,
	struct cq_desc *cq_desc, u16 completed_index,
	void (*buf_service)(struct vnic_wq *wq,
	struct cq_desc *cq_desc, struct vnic_wq_buf *buf, void *opaque),
	void *opaque)
{
	struct vnic_wq_buf *buf;

	buf = wq->to_clean;
	while (1) {

		(*buf_service)(wq, cq_desc, buf, opaque);

		wq->ring.desc_avail++;

		wq->to_clean = buf->next;

		if (buf->index == completed_index)
			break;

		buf = wq->to_clean;
	}
}

void svnic_wq_free(struct vnic_wq *wq);
int svnic_wq_alloc(struct vnic_dev *vdev, struct vnic_wq *wq,
	unsigned int index, unsigned int desc_count, unsigned int desc_size);
int vnic_wq_devcmd2_alloc(struct vnic_dev *vdev, struct vnic_wq *wq,
		unsigned int desc_count, unsigned int desc_size);
void vnic_wq_init_start(struct vnic_wq *wq, unsigned int cq_index,
		unsigned int fetch_index, unsigned int post_index,
		unsigned int error_interrupt_enable,
		unsigned int error_interrupt_offset);

void svnic_wq_init(struct vnic_wq *wq, unsigned int cq_index,
	unsigned int error_interrupt_enable,
	unsigned int error_interrupt_offset);
unsigned int svnic_wq_error_status(struct vnic_wq *wq);
void svnic_wq_enable(struct vnic_wq *wq);
int svnic_wq_disable(struct vnic_wq *wq);
void svnic_wq_clean(struct vnic_wq *wq,
	void (*buf_clean)(struct vnic_wq *wq, struct vnic_wq_buf *buf));
#endif /* _VNIC_WQ_H_ */
