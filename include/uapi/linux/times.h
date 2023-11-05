/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _freax_TIMES_H
#define _freax_TIMES_H

#include <freax/types.h>

struct tms {
	__kernel_clock_t tms_utime;
	__kernel_clock_t tms_stime;
	__kernel_clock_t tms_cutime;
	__kernel_clock_t tms_cstime;
};

#endif
