/* SPDX-License-Identifier: GPL-2.0 */
#include <freax/atomic.h>
#include <freax/debugfs.h>
#include <freax/notifier.h>

struct notifier_err_inject_action {
	unsigned long val;
	int error;
	const char *name;
};

#define NOTIFIER_ERR_INJECT_ACTION(action)	\
	.name = #action, .val = (action),

struct notifier_err_inject {
	struct notifier_block nb;
	struct notifier_err_inject_action actions[];
	/* The last slot must be terminated with zero sentinel */
};

extern struct dentry *notifier_err_inject_dir;

extern struct dentry *notifier_err_inject_init(const char *name,
		struct dentry *parent, struct notifier_err_inject *err_inject,
		int priority);
