// SPDX-License-Identifier: GPL-2.0
/*
 * Author: Huacai Chen <chenhuacai@loongson.cn>
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */

#include <freax/binfmts.h>
#include <freax/elf.h>
#include <freax/export.h>
#include <freax/sched.h>

#include <asm/cpu-features.h>
#include <asm/cpu-info.h>

int arch_elf_pt_proc(void *_ehdr, void *_phdr, struct file *elf,
		     bool is_interp, struct arch_elf_state *state)
{
	return 0;
}

int arch_check_elf(void *_ehdr, bool has_interpreter, void *_interp_ehdr,
		   struct arch_elf_state *state)
{
	return 0;
}

void loongarch_set_personality_fcsr(struct arch_elf_state *state)
{
	current->thread.fpu.fcsr = boot_cpu_data.fpu_csr0;
}
