/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ARM_SYSTEM_MISC_H
#define __ASM_ARM_SYSTEM_MISC_H

#ifndef __ASSEMBLY__

#include <freax/compiler.h>
#include <freax/linkage.h>
#include <freax/irqflags.h>
#include <freax/reboot.h>
#include <freax/percpu.h>

extern void cpu_init(void);

void soft_restart(unsigned long);
extern void (*arm_pm_idle)(void);

#ifdef CONFIG_HARDEN_BRANCH_PREDICTOR
typedef void (*harden_branch_predictor_fn_t)(void);
DECLARE_PER_CPU(harden_branch_predictor_fn_t, harden_branch_predictor_fn);
static inline void harden_branch_predictor(void)
{
	harden_branch_predictor_fn_t fn = per_cpu(harden_branch_predictor_fn,
						  smp_processor_id());
	if (fn)
		fn();
}
#else
#define harden_branch_predictor() do { } while (0)
#endif

#define UDBG_UNDEFINED	(1 << 0)
#define UDBG_SYSCALL	(1 << 1)
#define UDBG_BADABORT	(1 << 2)
#define UDBG_SEGV	(1 << 3)
#define UDBG_BUS	(1 << 4)

extern unsigned int user_debug;

#endif /* !__ASSEMBLY__ */

#endif /* __ASM_ARM_SYSTEM_MISC_H */
