// SPDX-License-Identifier: GPL-2.0
/*
 * Performance event callchain support - SuperH architecture code
 *
 * Copyright (C) 2009  Paul Mundt
 */
#include <freax/kernel.h>
#include <freax/sched.h>
#include <freax/perf_event.h>
#include <freax/percpu.h>
#include <asm/unwinder.h>
#include <asm/ptrace.h>

static void callchain_address(void *data, unsigned long addr, int reliable)
{
	struct perf_callchain_entry_ctx *entry = data;

	if (reliable)
		perf_callchain_store(entry, addr);
}

static const struct stacktrace_ops callchain_ops = {
	.address	= callchain_address,
};

void
perf_callchain_kernel(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs)
{
	perf_callchain_store(entry, regs->pc);

	unwind_stack(NULL, regs, NULL, &callchain_ops, entry);
}
