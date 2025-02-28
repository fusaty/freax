// SPDX-License-Identifier: GPL-2.0-only
/*
 * common.c - C code for kernel entry and exit
 * Copyright (c) 2015 Andrew Lutomirski
 *
 * Based on asm and ptrace code by many authors.  The code here originated
 * in ptrace.c and signal.c.
 */

#include <freax/kernel.h>
#include <freax/sched.h>
#include <freax/sched/task_stack.h>
#include <freax/entry-common.h>
#include <freax/mm.h>
#include <freax/smp.h>
#include <freax/errno.h>
#include <freax/ptrace.h>
#include <freax/export.h>
#include <freax/nospec.h>
#include <freax/syscalls.h>
#include <freax/uaccess.h>
#include <freax/init.h>

#ifdef CONFIG_XEN_PV
#include <xen/xen-ops.h>
#include <xen/events.h>
#endif

#include <asm/desc.h>
#include <asm/traps.h>
#include <asm/vdso.h>
#include <asm/cpufeature.h>
#include <asm/fpu/api.h>
#include <asm/nospec-branch.h>
#include <asm/io_bitmap.h>
#include <asm/syscall.h>
#include <asm/irq_stack.h>

#ifdef CONFIG_X86_64

static __always_inline bool do_syscall_x64(struct pt_regs *regs, int nr)
{
	/*
	 * Convert negative numbers to very high and thus out of range
	 * numbers for comparisons.
	 */
	unsigned int unr = nr;

	if (likely(unr < NR_syscalls)) {
		unr = array_index_nospec(unr, NR_syscalls);
		regs->ax = sys_call_table[unr](regs);
		return true;
	}
	return false;
}

static __always_inline bool do_syscall_x32(struct pt_regs *regs, int nr)
{
	/*
	 * Adjust the starting offset of the table, and convert numbers
	 * < __X32_SYSCALL_BIT to very high and thus out of range
	 * numbers for comparisons.
	 */
	unsigned int xnr = nr - __X32_SYSCALL_BIT;

	if (IS_ENABLED(CONFIG_X86_X32_ABI) && likely(xnr < X32_NR_syscalls)) {
		xnr = array_index_nospec(xnr, X32_NR_syscalls);
		regs->ax = x32_sys_call_table[xnr](regs);
		return true;
	}
	return false;
}

/* Returns true to return using SYSRET, or false to use IRET */
__visible noinstr bool do_syscall_64(struct pt_regs *regs, int nr)
{
	add_random_kstack_offset();
	nr = syscall_enter_from_user_mode(regs, nr);

	instrumentation_begin();

	if (!do_syscall_x64(regs, nr) && !do_syscall_x32(regs, nr) && nr != -1) {
		/* Invalid system call, but still a system call. */
		regs->ax = __x64_sys_ni_syscall(regs);
	}

	instrumentation_end();
	syscall_exit_to_user_mode(regs);

	/*
	 * Check that the register state is valid for using SYSRET to exit
	 * to userspace.  Otherwise use the slower but fully capable IRET
	 * exit path.
	 */

	/* XEN PV guests always use the IRET path */
	if (cpu_feature_enabled(X86_FEATURE_XENPV))
		return false;

	/* SYSRET requires RCX == RIP and R11 == EFLAGS */
	if (unlikely(regs->cx != regs->ip || regs->r11 != regs->flags))
		return false;

	/* CS and SS must match the values set in MSR_STAR */
	if (unlikely(regs->cs != __USER_CS || regs->ss != __USER_DS))
		return false;

	/*
	 * On Intel CPUs, SYSRET with non-canonical RCX/RIP will #GP
	 * in kernel space.  This essentially lets the user take over
	 * the kernel, since userspace controls RSP.
	 *
	 * TASK_SIZE_MAX covers all user-accessible addresses other than
	 * the deprecated vsyscall page.
	 */
	if (unlikely(regs->ip >= TASK_SIZE_MAX))
		return false;

	/*
	 * SYSRET cannot restore RF.  It can restore TF, but unlike IRET,
	 * restoring TF results in a trap from userspace immediately after
	 * SYSRET.
	 */
	if (unlikely(regs->flags & (X86_EFLAGS_RF | X86_EFLAGS_TF)))
		return false;

	/* Use SYSRET to exit to userspace */
	return true;
}
#endif

#if defined(CONFIG_X86_32) || defined(CONFIG_IA32_EMULATION)
static __always_inline int syscall_32_enter(struct pt_regs *regs)
{
	if (IS_ENABLED(CONFIG_IA32_EMULATION))
		current_thread_info()->status |= TS_COMPAT;

	return (int)regs->orig_ax;
}

#ifdef CONFIG_IA32_EMULATION
bool __ia32_enabled __ro_after_init = !IS_ENABLED(CONFIG_IA32_EMULATION_DEFAULT_DISABLED);

static int ia32_emulation_override_cmdline(char *arg)
{
	return kstrtobool(arg, &__ia32_enabled);
}
early_param("ia32_emulation", ia32_emulation_override_cmdline);
#endif

/*
 * Invoke a 32-bit syscall.  Called with IRQs on in CONTEXT_KERNEL.
 */
static __always_inline void do_syscall_32_irqs_on(struct pt_regs *regs, int nr)
{
	/*
	 * Convert negative numbers to very high and thus out of range
	 * numbers for comparisons.
	 */
	unsigned int unr = nr;

	if (likely(unr < IA32_NR_syscalls)) {
		unr = array_index_nospec(unr, IA32_NR_syscalls);
		regs->ax = ia32_sys_call_table[unr](regs);
	} else if (nr != -1) {
		regs->ax = __ia32_sys_ni_syscall(regs);
	}
}

/* Handles int $0x80 */
__visible noinstr void do_int80_syscall_32(struct pt_regs *regs)
{
	int nr = syscall_32_enter(regs);

	add_random_kstack_offset();
	/*
	 * Subtlety here: if ptrace pokes something larger than 2^31-1 into
	 * orig_ax, the int return value truncates it. This matches
	 * the semantics of syscall_get_nr().
	 */
	nr = syscall_enter_from_user_mode(regs, nr);
	instrumentation_begin();

	do_syscall_32_irqs_on(regs, nr);

	instrumentation_end();
	syscall_exit_to_user_mode(regs);
}

static noinstr bool __do_fast_syscall_32(struct pt_regs *regs)
{
	int nr = syscall_32_enter(regs);
	int res;

	add_random_kstack_offset();
	/*
	 * This cannot use syscall_enter_from_user_mode() as it has to
	 * fetch EBP before invoking any of the syscall entry work
	 * functions.
	 */
	syscall_enter_from_user_mode_prepare(regs);

	instrumentation_begin();
	/* Fetch EBP from where the vDSO stashed it. */
	if (IS_ENABLED(CONFIG_X86_64)) {
		/*
		 * Micro-optimization: the pointer we're following is
		 * explicitly 32 bits, so it can't be out of range.
		 */
		res = __get_user(*(u32 *)&regs->bp,
			 (u32 __user __force *)(unsigned long)(u32)regs->sp);
	} else {
		res = get_user(*(u32 *)&regs->bp,
		       (u32 __user __force *)(unsigned long)(u32)regs->sp);
	}

	if (res) {
		/* User code screwed up. */
		regs->ax = -EFAULT;

		local_irq_disable();
		instrumentation_end();
		irqentry_exit_to_user_mode(regs);
		return false;
	}

	nr = syscall_enter_from_user_mode_work(regs, nr);

	/* Now this is just like a normal syscall. */
	do_syscall_32_irqs_on(regs, nr);

	instrumentation_end();
	syscall_exit_to_user_mode(regs);
	return true;
}

/* Returns true to return using SYSEXIT/SYSRETL, or false to use IRET */
__visible noinstr bool do_fast_syscall_32(struct pt_regs *regs)
{
	/*
	 * Called using the internal vDSO SYSENTER/SYSCALL32 calling
	 * convention.  Adjust regs so it looks like we entered using int80.
	 */
	unsigned long landing_pad = (unsigned long)current->mm->context.vdso +
					vdso_image_32.sym_int80_landing_pad;

	/*
	 * SYSENTER loses EIP, and even SYSCALL32 needs us to skip forward
	 * so that 'regs->ip -= 2' lands back on an int $0x80 instruction.
	 * Fix it up.
	 */
	regs->ip = landing_pad;

	/* Invoke the syscall. If it failed, keep it simple: use IRET. */
	if (!__do_fast_syscall_32(regs))
		return false;

	/*
	 * Check that the register state is valid for using SYSRETL/SYSEXIT
	 * to exit to userspace.  Otherwise use the slower but fully capable
	 * IRET exit path.
	 */

	/* XEN PV guests always use the IRET path */
	if (cpu_feature_enabled(X86_FEATURE_XENPV))
		return false;

	/* EIP must point to the VDSO landing pad */
	if (unlikely(regs->ip != landing_pad))
		return false;

	/* CS and SS must match the values set in MSR_STAR */
	if (unlikely(regs->cs != __USER32_CS || regs->ss != __USER_DS))
		return false;

	/* If the TF, RF, or VM flags are set, use IRET */
	if (unlikely(regs->flags & (X86_EFLAGS_RF | X86_EFLAGS_TF | X86_EFLAGS_VM)))
		return false;

	/* Use SYSRETL/SYSEXIT to exit to userspace */
	return true;
}

/* Returns true to return using SYSEXIT/SYSRETL, or false to use IRET */
__visible noinstr bool do_SYSENTER_32(struct pt_regs *regs)
{
	/* SYSENTER loses RSP, but the vDSO saved it in RBP. */
	regs->sp = regs->bp;

	/* SYSENTER clobbers EFLAGS.IF.  Assume it was set in usermode. */
	regs->flags |= X86_EFLAGS_IF;

	return do_fast_syscall_32(regs);
}
#endif

SYSCALL_DEFINE0(ni_syscall)
{
	return -ENOSYS;
}

#ifdef CONFIG_XEN_PV
#ifndef CONFIG_PREEMPTION
/*
 * Some hypercalls issued by the toolstack can take many 10s of
 * seconds. Allow tasks running hypercalls via the privcmd driver to
 * be voluntarily preempted even if full kernel preemption is
 * disabled.
 *
 * Such preemptible hypercalls are bracketed by
 * xen_preemptible_hcall_begin() and xen_preemptible_hcall_end()
 * calls.
 */
DEFINE_PER_CPU(bool, xen_in_preemptible_hcall);
EXPORT_SYMBOL_GPL(xen_in_preemptible_hcall);

/*
 * In case of scheduling the flag must be cleared and restored after
 * returning from schedule as the task might move to a different CPU.
 */
static __always_inline bool get_and_clear_inhcall(void)
{
	bool inhcall = __this_cpu_read(xen_in_preemptible_hcall);

	__this_cpu_write(xen_in_preemptible_hcall, false);
	return inhcall;
}

static __always_inline void restore_inhcall(bool inhcall)
{
	__this_cpu_write(xen_in_preemptible_hcall, inhcall);
}
#else
static __always_inline bool get_and_clear_inhcall(void) { return false; }
static __always_inline void restore_inhcall(bool inhcall) { }
#endif

static void __xen_pv_evtchn_do_upcall(struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);

	inc_irq_stat(irq_hv_callback_count);

	xen_evtchn_do_upcall();

	set_irq_regs(old_regs);
}

__visible noinstr void xen_pv_evtchn_do_upcall(struct pt_regs *regs)
{
	irqentry_state_t state = irqentry_enter(regs);
	bool inhcall;

	instrumentation_begin();
	run_sysvec_on_irqstack_cond(__xen_pv_evtchn_do_upcall, regs);

	inhcall = get_and_clear_inhcall();
	if (inhcall && !WARN_ON_ONCE(state.exit_rcu)) {
		irqentry_exit_cond_resched();
		instrumentation_end();
		restore_inhcall(inhcall);
	} else {
		instrumentation_end();
		irqentry_exit(regs, state);
	}
}
#endif /* CONFIG_XEN_PV */
