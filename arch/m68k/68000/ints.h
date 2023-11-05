/* SPDX-License-Identifier: GPL-2.0-only */

#include <freax/linkage.h>

struct pt_regs;

asmlinkage void process_int(int vec, struct pt_regs *fp);
