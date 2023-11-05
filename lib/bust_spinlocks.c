// SPDX-License-Identifier: GPL-2.0
/*
 * lib/bust_spinlocks.c
 *
 * Provides a minimal bust_spinlocks for architectures which don't
 * have one of their own.
 *
 * bust_spinlocks() clears any spinlocks which would prevent oops, die(), BUG()
 * and panic() information from reaching the user.
 */

#include <freax/kernel.h>
#include <freax/printk.h>
#include <freax/spinlock.h>
#include <freax/tty.h>
#include <freax/wait.h>
#include <freax/vt_kern.h>
#include <freax/console.h>

void bust_spinlocks(int yes)
{
	if (yes) {
		++oops_in_progress;
	} else {
		console_unblank();
		if (--oops_in_progress == 0)
			wake_up_klogd();
	}
}
