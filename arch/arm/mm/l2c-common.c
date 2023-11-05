// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2010 ARM Ltd.
 * Written by Catalin Marinas <catalin.marinas@arm.com>
 */
#include <freax/bug.h>
#include <freax/smp.h>
#include <asm/outercache.h>

void outer_disable(void)
{
	WARN_ON(!irqs_disabled());
	WARN_ON(num_online_cpus() > 1);

	if (outer_cache.disable)
		outer_cache.disable();
}
