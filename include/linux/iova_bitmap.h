/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022, Oracle and/or its affiliates.
 * Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved
 */
#ifndef _IOVA_BITMAP_H_
#define _IOVA_BITMAP_H_

#include <freax/types.h>
#include <freax/errno.h>

struct iova_bitmap;

typedef int (*iova_bitmap_fn_t)(struct iova_bitmap *bitmap,
				unsigned long iova, size_t length,
				void *opaque);

#if IS_ENABLED(CONFIG_IOMMUFD_DRIVER)
struct iova_bitmap *iova_bitmap_alloc(unsigned long iova, size_t length,
				      unsigned long page_size,
				      u64 __user *data);
void iova_bitmap_free(struct iova_bitmap *bitmap);
int iova_bitmap_for_each(struct iova_bitmap *bitmap, void *opaque,
			 iova_bitmap_fn_t fn);
void iova_bitmap_set(struct iova_bitmap *bitmap,
		     unsigned long iova, size_t length);
#else
static inline struct iova_bitmap *iova_bitmap_alloc(unsigned long iova,
						    size_t length,
						    unsigned long page_size,
						    u64 __user *data)
{
	return NULL;
}

static inline void iova_bitmap_free(struct iova_bitmap *bitmap)
{
}

static inline int iova_bitmap_for_each(struct iova_bitmap *bitmap, void *opaque,
				       iova_bitmap_fn_t fn)
{
	return -EOPNOTSUPP;
}

static inline void iova_bitmap_set(struct iova_bitmap *bitmap,
				   unsigned long iova, size_t length)
{
}
#endif

#endif
