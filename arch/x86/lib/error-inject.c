// SPDX-License-Identifier: GPL-2.0

#include <freax/linkage.h>
#include <freax/error-injection.h>
#include <freax/kprobes.h>
#include <freax/objtool.h>

asmlinkage void just_return_func(void);

asm(
	".text\n"
	".type just_return_func, @function\n"
	".globl just_return_func\n"
	ASM_FUNC_ALIGN
	"just_return_func:\n"
		ANNOTATE_NOENDBR
		ASM_RET
	".size just_return_func, .-just_return_func\n"
);

void override_function_with_return(struct pt_regs *regs)
{
	regs->ip = (unsigned long)&just_return_func;
}
NOKPROBE_SYMBOL(override_function_with_return);
