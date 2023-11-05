// SPDX-License-Identifier: GPL-2.0-only
/*
 * These are various utility functions of the scheduler,
 * built in a single compilation unit for build efficiency reasons.
 *
 * ( Incidentally, the size of the compilation unit is roughly
 *   comparable to core.c, fair.c, smp.c and policy.c, the other
 *   big compilation units. This helps balance build time, while
 *   coalescing source files to amortize header inclusion
 *   cost. )
 */
#include <freax/sched/clock.h>
#include <freax/sched/cputime.h>
#include <freax/sched/debug.h>
#include <freax/sched/isolation.h>
#include <freax/sched/loadavg.h>
#include <freax/sched/nohz.h>
#include <freax/sched/mm.h>
#include <freax/sched/rseq_api.h>
#include <freax/sched/task_stack.h>

#include <freax/cpufreq.h>
#include <freax/cpumask_api.h>
#include <freax/cpuset.h>
#include <freax/ctype.h>
#include <freax/debugfs.h>
#include <freax/energy_model.h>
#include <freax/hashtable_api.h>
#include <freax/irq.h>
#include <freax/kobject_api.h>
#include <freax/membarrier.h>
#include <freax/mempolicy.h>
#include <freax/nmi.h>
#include <freax/nospec.h>
#include <freax/proc_fs.h>
#include <freax/psi.h>
#include <freax/ptrace_api.h>
#include <freax/sched_clock.h>
#include <freax/security.h>
#include <freax/spinlock_api.h>
#include <freax/swait_api.h>
#include <freax/timex.h>
#include <freax/utsname.h>
#include <freax/wait_api.h>
#include <freax/workqueue_api.h>

#include <uapi/freax/prctl.h>
#include <uapi/freax/sched/types.h>

#include <asm/switch_to.h>

#include "sched.h"
#include "sched-pelt.h"
#include "stats.h"
#include "autogroup.h"

#include "clock.c"

#ifdef CONFIG_CGROUP_CPUACCT
# include "cpuacct.c"
#endif

#ifdef CONFIG_CPU_FREQ
# include "cpufreq.c"
#endif

#ifdef CONFIG_CPU_FREQ_GOV_SCHEDUTIL
# include "cpufreq_schedutil.c"
#endif

#ifdef CONFIG_SCHED_DEBUG
# include "debug.c"
#endif

#ifdef CONFIG_SCHEDSTATS
# include "stats.c"
#endif

#include "loadavg.c"
#include "completion.c"
#include "swait.c"
#include "wait_bit.c"
#include "wait.c"

#ifdef CONFIG_SMP
# include "cpupri.c"
# include "stop_task.c"
# include "topology.c"
#endif

#ifdef CONFIG_SCHED_CORE
# include "core_sched.c"
#endif

#ifdef CONFIG_PSI
# include "psi.c"
#endif

#ifdef CONFIG_MEMBARRIER
# include "membarrier.c"
#endif

#ifdef CONFIG_CPU_ISOLATION
# include "isolation.c"
#endif

#ifdef CONFIG_SCHED_AUTOGROUP
# include "autogroup.c"
#endif
