/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Driver for the Conexant CX25821 PCIe bridge
 *
 *  Copyright (C) 2009 Conexant Systems Inc.
 *  Authors  <shu.lin@conexant.com>, <hiep.huynh@conexant.com>
 *  Based on Steven Toth <stoth@freaxtv.org> cx23885 driver
 */

#ifndef CX25821_VIDEO_H_
#define CX25821_VIDEO_H_

#include <freax/init.h>
#include <freax/list.h>
#include <freax/module.h>
#include <freax/moduleparam.h>
#include <freax/kmod.h>
#include <freax/kernel.h>
#include <freax/slab.h>
#include <freax/interrupt.h>
#include <freax/delay.h>
#include <freax/kthread.h>
#include <asm/div64.h>

#include "cx25821.h"
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>

#define VIDEO_DEBUG 0

#define dprintk(level, fmt, arg...)					\
do {									\
	if (VIDEO_DEBUG >= level)					\
		printk(KERN_DEBUG "%s/0: " fmt, dev->name, ##arg);	\
} while (0)

#define FORMAT_FLAGS_PACKED       0x01
extern int cx25821_start_video_dma(struct cx25821_dev *dev,
				   struct cx25821_dmaqueue *q,
				   struct cx25821_buffer *buf,
				   const struct sram_channel *channel);

extern int cx25821_video_irq(struct cx25821_dev *dev, int chan_num, u32 status);
extern void cx25821_video_unregister(struct cx25821_dev *dev, int chan_num);
extern int cx25821_video_register(struct cx25821_dev *dev);

#endif
