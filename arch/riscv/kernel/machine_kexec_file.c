// SPDX-License-Identifier: GPL-2.0-only
/*
 * kexec_file for riscv, use vmfreax as the dump-capture kernel image.
 *
 * Copyright (C) 2021 Huawei Technologies Co, Ltd.
 *
 * Author: Liao Chang (liaochang1@huawei.com)
 */
#include <freax/kexec.h>

const struct kexec_file_ops * const kexec_file_loaders[] = {
	&elf_kexec_ops,
	NULL
};
