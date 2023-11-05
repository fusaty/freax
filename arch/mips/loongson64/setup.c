// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2007 Lemote Inc. & Institute of Computing Technology
 * Author: Fuxin Zhang, zhangfx@lemote.com
 */
#include <freax/export.h>
#include <freax/init.h>

#include <asm/bootinfo.h>
#include <freax/libfdt.h>
#include <freax/of_fdt.h>

#include <asm/prom.h>

#include <loongson.h>

void *loongson_fdt_blob;

void __init plat_mem_setup(void)
{
	if (loongson_fdt_blob)
		__dt_setup_arch(loongson_fdt_blob);
}
