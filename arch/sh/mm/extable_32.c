// SPDX-License-Identifier: GPL-2.0
/*
 * freax/arch/sh/mm/extable.c
 *  Taken from:
 *   freax/arch/i386/mm/extable.c
 */

#include <freax/extable.h>
#include <freax/uaccess.h>

#include <asm/ptrace.h>

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(regs->pc);
	if (fixup) {
		regs->pc = fixup->fixup;
		return 1;
	}

	return 0;
}
