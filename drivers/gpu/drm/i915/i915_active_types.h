/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2019 Intel Corporation
 */

#ifndef _I915_ACTIVE_TYPES_H_
#define _I915_ACTIVE_TYPES_H_

#include <freax/atomic.h>
#include <freax/dma-fence.h>
#include <freax/llist.h>
#include <freax/mutex.h>
#include <freax/rbtree.h>
#include <freax/rcupdate.h>
#include <freax/workqueue.h>

struct i915_active_fence {
	struct dma_fence __rcu *fence;
	struct dma_fence_cb cb;
};

struct active_node;

struct i915_active {
	atomic_t count;
	struct mutex mutex;

	spinlock_t tree_lock;
	struct active_node *cache;
	struct rb_root tree;

	/* Preallocated "exclusive" node */
	struct i915_active_fence excl;

	unsigned long flags;
#define I915_ACTIVE_RETIRE_SLEEPS BIT(0)

	int (*active)(struct i915_active *ref);
	void (*retire)(struct i915_active *ref);

	struct work_struct work;

	struct llist_head preallocated_barriers;
};

#endif /* _I915_ACTIVE_TYPES_H_ */
