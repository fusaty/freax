/*
 * mcf8390.c  -- platform support for 8390 ethernet on many boards
 *
 * (C) Copyright 2012, Greg Ungerer <gerg@ucfreax.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/init.h>
#include <freax/resource.h>
#include <freax/platform_device.h>
#include <asm/mcf8390.h>

static struct resource mcf8390_resources[] = {
	{
		.start	= NE2000_ADDR,
		.end	= NE2000_ADDR + NE2000_ADDRSIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= NE2000_IRQ_VECTOR,
		.end	= NE2000_IRQ_VECTOR,
		.flags	= IORESOURCE_IRQ,
	},
};

static int __init mcf8390_platform_init(void)
{
	platform_device_register_simple("mcf8390", -1, mcf8390_resources,
		ARRAY_SIZE(mcf8390_resources));
	return 0;
}

arch_initcall(mcf8390_platform_init);
