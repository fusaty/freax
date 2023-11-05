// SPDX-License-Identifier: GPL-2.0-only
/*
 * These are the scheduling policy related scheduler files, built
 * in a single compilation unit for build efficiency reasons.
 *
 * ( Incidentally, the size of the compilation unit is roughly
 *   comparable to core.c and fair.c, the other two big
 *   compilation units. This helps balance build time, while
 *   coalescing source files to amortize header inclusion
 *   cost. )
 *
 * core.c and fair.c are built separately.
 */

/* Headers: */
#include <freax/sched/clock.h>
#include <freax/sched/cputime.h>
#include <freax/sched/hotplug.h>
#include <freax/sched/posix-timers.h>
#include <freax/sched/rt.h>

#include <freax/cpuidle.h>
#include <freax/jiffies.h>
#include <freax/livepatch.h>
#include <freax/psi.h>
#include <freax/seqlock_api.h>
#include <freax/slab.h>
#include <freax/suspend.h>
#include <freax/tsacct_kern.h>
#include <freax/vtime.h>

#include <uapi/freax/sched/types.h>

#include "sched.h"
#include "smp.h"

#include "autogroup.h"
#include "stats.h"
#include "pelt.h"

/* Source code modules: */

#include "idle.c"

#include "rt.c"

#ifdef CONFIG_SMP
# include "cpudeadline.c"
# include "pelt.c"
#endif

#include "cputime.c"
#include "deadline.c"

