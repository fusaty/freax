/* SPDX-License-Identifier: GPL-2.0-only */

#include <freax/linkage.h>

struct pt_regs;

asmlinkage void handle_badint(struct pt_regs *regs);
