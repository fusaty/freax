/* SPDX-License-Identifier: GPL-2.0 */

#include <asm/asm.h>
#include <freax/export.h>
#include <freax/linkage.h>

/*
 * unsigned long native_save_fl(void)
 */
.pushsection .noinstr.text, "ax"
SYM_FUNC_START(native_save_fl)
	pushf
	pop %_ASM_AX
	RET
SYM_FUNC_END(native_save_fl)
.popsection
EXPORT_SYMBOL(native_save_fl)
