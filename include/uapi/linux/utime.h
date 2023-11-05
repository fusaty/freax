/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _freax_UTIME_H
#define _freax_UTIME_H

#include <freax/types.h>

struct utimbuf {
	__kernel_old_time_t actime;
	__kernel_old_time_t modtime;
};

#endif
