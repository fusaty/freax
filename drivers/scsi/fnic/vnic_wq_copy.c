// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2008 Cisco Systems, Inc.  All rights reserved.
 * Copyright 2007 Nuova Systems, Inc.  All rights reserved.
 */

#include <freax/errno.h>
#include <freax/types.h>
#include <freax/pci.h>
#include <freax/delay.h>
#include "vnic_wq_copy.h"

void vnic_wq_copy_enable(struct vnic_wq_copy *wq)
{
	iowrite32(1, &wq->ctrl->enable);
}

int vnic_wq_copy_disable(struct vnic_wq_copy *wq)
{
	unsigned int wait;

	iowrite32(0, &wq->ctrl->enable);

	/* Wait for HW to ACK disable request */
	for (wait = 0; wait < 100; wait++) {
		if (!(ioread32(&wq->ctrl->running)))
			return 0;
		udelay(1);
	}

	printk(KERN_ERR "Failed to disable Copy WQ[%d],"
	       " fetch index=%d, posted_index=%d\n",
	       wq->index, ioread32(&wq->ctrl->fetch_index),
	       ioread32(&wq->ctrl->posted_index));

	return -ENODEV;
}

void vnic_wq_copy_clean(struct vnic_wq_copy *wq,
	void (*q_clean)(struct vnic_wq_copy *wq,
	struct fcpio_host_req *wq_desc))
{
	BUG_ON(ioread32(&wq->ctrl->enable));

	if (vnic_wq_copy_desc_in_use(wq))
		vnic_wq_copy_service(wq, -1, q_clean);

	wq->to_use_index = wq->to_clean_index = 0;

	iowrite32(0, &wq->ctrl->fetch_index);
	iowrite32(0, &wq->ctrl->posted_index);
	iowrite32(0, &wq->ctrl->error_status);

	vnic_dev_clear_desc_ring(&wq->ring);
}

void vnic_wq_copy_free(struct vnic_wq_copy *wq)
{
	struct vnic_dev *vdev;

	vdev = wq->vdev;
	vnic_dev_free_desc_ring(vdev, &wq->ring);
	wq->ctrl = NULL;
}

int vnic_wq_copy_alloc(struct vnic_dev *vdev, struct vnic_wq_copy *wq,
		       unsigned int index, unsigned int desc_count,
		       unsigned int desc_size)
{
	wq->index = index;
	wq->vdev = vdev;
	wq->to_use_index = wq->to_clean_index = 0;
	wq->ctrl = vnic_dev_get_res(vdev, RES_TYPE_WQ, index);
	if (!wq->ctrl) {
		printk(KERN_ERR "Failed to hook COPY WQ[%d] resource\n", index);
		return -EINVAL;
	}

	vnic_wq_copy_disable(wq);

	return vnic_dev_alloc_desc_ring(vdev, &wq->ring, desc_count, desc_size);
}

void vnic_wq_copy_init(struct vnic_wq_copy *wq, unsigned int cq_index,
	unsigned int error_interrupt_enable,
	unsigned int error_interrupt_offset)
{
	u64 paddr;

	paddr = (u64)wq->ring.base_addr | VNIC_PADDR_TARGET;
	writeq(paddr, &wq->ctrl->ring_base);
	iowrite32(wq->ring.desc_count, &wq->ctrl->ring_size);
	iowrite32(0, &wq->ctrl->fetch_index);
	iowrite32(0, &wq->ctrl->posted_index);
	iowrite32(cq_index, &wq->ctrl->cq_index);
	iowrite32(error_interrupt_enable, &wq->ctrl->error_interrupt_enable);
	iowrite32(error_interrupt_offset, &wq->ctrl->error_interrupt_offset);
}
