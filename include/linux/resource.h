/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_RESOURCE_H
#define _freax_RESOURCE_H

#include <uapi/freax/resource.h>


struct task_struct;

void getrusage(struct task_struct *p, int who, struct rusage *ru);

#endif
