// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mapping of DWARF debug register numbers into register names.
 *
 * Copyright (C) 2010 Will Deacon, ARM Ltd.
 */

#include <stddef.h>
#include <freax/stringify.h>
#include <dwarf-regs.h>

struct pt_regs_dwarfnum {
	const char *name;
	unsigned int dwarfnum;
};

#define REG_DWARFNUM_NAME(r, num) {.name = r, .dwarfnum = num}
#define GPR_DWARFNUM_NAME(num) \
	{.name = __stringify(%r##num), .dwarfnum = num}
#define REG_DWARFNUM_END {.name = NULL, .dwarfnum = 0}

/*
 * Reference:
 * http://infocenter.arm.com/help/topic/com.arm.doc.ihi0040a/IHI0040A_aadwarf.pdf
 */
static const struct pt_regs_dwarfnum regdwarfnum_table[] = {
	GPR_DWARFNUM_NAME(0),
	GPR_DWARFNUM_NAME(1),
	GPR_DWARFNUM_NAME(2),
	GPR_DWARFNUM_NAME(3),
	GPR_DWARFNUM_NAME(4),
	GPR_DWARFNUM_NAME(5),
	GPR_DWARFNUM_NAME(6),
	GPR_DWARFNUM_NAME(7),
	GPR_DWARFNUM_NAME(8),
	GPR_DWARFNUM_NAME(9),
	GPR_DWARFNUM_NAME(10),
	REG_DWARFNUM_NAME("%fp", 11),
	REG_DWARFNUM_NAME("%ip", 12),
	REG_DWARFNUM_NAME("%sp", 13),
	REG_DWARFNUM_NAME("%lr", 14),
	REG_DWARFNUM_NAME("%pc", 15),
	REG_DWARFNUM_END,
};

/**
 * get_arch_regstr() - lookup register name from it's DWARF register number
 * @n:	the DWARF register number
 *
 * get_arch_regstr() returns the name of the register in struct
 * regdwarfnum_table from it's DWARF register number. If the register is not
 * found in the table, this returns NULL;
 */
const char *get_arch_regstr(unsigned int n)
{
	const struct pt_regs_dwarfnum *roff;
	for (roff = regdwarfnum_table; roff->name != NULL; roff++)
		if (roff->dwarfnum == n)
			return roff->name;
	return NULL;
}
