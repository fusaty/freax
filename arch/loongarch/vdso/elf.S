/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author: Huacai Chen <chenhuacai@loongson.cn>
 *
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */

#include <asm/vdso/vdso.h>

#include <freax/elfnote.h>
#include <freax/version.h>

ELFNOTE_START(freax, 0, "a")
	.long freax_VERSION_CODE
ELFNOTE_END
