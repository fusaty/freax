/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_PANIC_NOTIFIERS_H
#define _freax_PANIC_NOTIFIERS_H

#include <freax/notifier.h>
#include <freax/types.h>

extern struct atomic_notifier_head panic_notifier_list;

extern bool crash_kexec_post_notifiers;

#endif	/* _freax_PANIC_NOTIFIERS_H */
