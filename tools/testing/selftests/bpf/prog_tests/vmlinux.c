// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2020 Facebook */

#include <test_progs.h>
#include <time.h>
#include "test_vmfreax.skel.h"

#define MY_TV_NSEC 1337

static void nsleep()
{
	struct timespec ts = { .tv_nsec = MY_TV_NSEC };

	(void)syscall(__NR_nanosleep, &ts, NULL);
}

void test_vmfreax(void)
{
	int duration = 0, err;
	struct test_vmfreax* skel;
	struct test_vmfreax__bss *bss;

	skel = test_vmfreax__open_and_load();
	if (CHECK(!skel, "skel_open", "failed to open skeleton\n"))
		return;
	bss = skel->bss;

	err = test_vmfreax__attach(skel);
	if (CHECK(err, "skel_attach", "skeleton attach failed: %d\n", err))
		goto cleanup;

	/* trigger everything */
	nsleep();

	CHECK(!bss->tp_called, "tp", "not called\n");
	CHECK(!bss->raw_tp_called, "raw_tp", "not called\n");
	CHECK(!bss->tp_btf_called, "tp_btf", "not called\n");
	CHECK(!bss->kprobe_called, "kprobe", "not called\n");
	CHECK(!bss->fentry_called, "fentry", "not called\n");

cleanup:
	test_vmfreax__destroy(skel);
}
