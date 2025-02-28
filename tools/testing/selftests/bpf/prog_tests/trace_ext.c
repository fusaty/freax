// SPDX-License-Identifier: GPL-2.0

#define _GNU_SOURCE
#include <test_progs.h>
#include <network_helpers.h>
#include <sys/stat.h>
#include <freax/sched.h>
#include <sys/syscall.h>

#include "test_pkt_md_access.skel.h"
#include "test_trace_ext.skel.h"
#include "test_trace_ext_tracing.skel.h"

static __u32 duration;

void test_trace_ext(void)
{
	struct test_pkt_md_access *skel_pkt = NULL;
	struct test_trace_ext_tracing *skel_trace = NULL;
	struct test_trace_ext_tracing__bss *bss_trace;
	struct test_trace_ext *skel_ext = NULL;
	struct test_trace_ext__bss *bss_ext;
	int err, pkt_fd, ext_fd;
	struct bpf_program *prog;
	char buf[100];
	__u64 len;
	LIBBPF_OPTS(bpf_test_run_opts, topts,
		.data_in = &pkt_v4,
		.data_size_in = sizeof(pkt_v4),
		.repeat = 1,
	);

	/* open/load/attach test_pkt_md_access */
	skel_pkt = test_pkt_md_access__open_and_load();
	if (CHECK(!skel_pkt, "setup", "classifier/test_pkt_md_access open failed\n"))
		goto cleanup;

	err = test_pkt_md_access__attach(skel_pkt);
	if (CHECK(err, "setup", "classifier/test_pkt_md_access attach failed: %d\n", err))
		goto cleanup;

	prog = skel_pkt->progs.test_pkt_md_access;
	pkt_fd = bpf_program__fd(prog);

	/* open extension */
	skel_ext = test_trace_ext__open();
	if (CHECK(!skel_ext, "setup", "freplace/test_pkt_md_access open failed\n"))
		goto cleanup;

	/* set extension's attach target - test_pkt_md_access  */
	prog = skel_ext->progs.test_pkt_md_access_new;
	bpf_program__set_attach_target(prog, pkt_fd, "test_pkt_md_access");

	/* load/attach extension */
	err = test_trace_ext__load(skel_ext);
	if (CHECK(err, "setup", "freplace/test_pkt_md_access load failed\n")) {
		libbpf_strerror(err, buf, sizeof(buf));
		fprintf(stderr, "%s\n", buf);
		goto cleanup;
	}

	err = test_trace_ext__attach(skel_ext);
	if (CHECK(err, "setup", "freplace/test_pkt_md_access attach failed: %d\n", err))
		goto cleanup;

	prog = skel_ext->progs.test_pkt_md_access_new;
	ext_fd = bpf_program__fd(prog);

	/* open tracing  */
	skel_trace = test_trace_ext_tracing__open();
	if (CHECK(!skel_trace, "setup", "tracing/test_pkt_md_access_new open failed\n"))
		goto cleanup;

	/* set tracing's attach target - fentry */
	prog = skel_trace->progs.fentry;
	bpf_program__set_attach_target(prog, ext_fd, "test_pkt_md_access_new");

	/* set tracing's attach target - fexit */
	prog = skel_trace->progs.fexit;
	bpf_program__set_attach_target(prog, ext_fd, "test_pkt_md_access_new");

	/* load/attach tracing */
	err = test_trace_ext_tracing__load(skel_trace);
	if (!ASSERT_OK(err, "tracing/test_pkt_md_access_new load")) {
		libbpf_strerror(err, buf, sizeof(buf));
		fprintf(stderr, "%s\n", buf);
		goto cleanup;
	}

	err = test_trace_ext_tracing__attach(skel_trace);
	if (!ASSERT_OK(err, "tracing/test_pkt_md_access_new attach"))
		goto cleanup;

	/* trigger the test */
	err = bpf_prog_test_run_opts(pkt_fd, &topts);
	ASSERT_OK(err, "test_run_opts err");
	ASSERT_OK(topts.retval, "test_run_opts retval");

	bss_ext = skel_ext->bss;
	bss_trace = skel_trace->bss;

	len = bss_ext->ext_called;

	ASSERT_NEQ(bss_ext->ext_called, 0,
		  "failed to trigger freplace/test_pkt_md_access");
	ASSERT_EQ(bss_trace->fentry_called, len,
		  "failed to trigger fentry/test_pkt_md_access_new");
	ASSERT_EQ(bss_trace->fexit_called, len,
		   "failed to trigger fexit/test_pkt_md_access_new");

cleanup:
	test_trace_ext_tracing__destroy(skel_trace);
	test_trace_ext__destroy(skel_ext);
	test_pkt_md_access__destroy(skel_pkt);
}
