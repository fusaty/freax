/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/freax/firmware-map.h:
 *  Copyright (C) 2008 SUSE freax Products GmbH
 *  by Bernhard Walle <bernhard.walle@gmx.de>
 */
#ifndef _freax_FIRMWARE_MAP_H
#define _freax_FIRMWARE_MAP_H

#include <freax/list.h>

/*
 * provide a dummy interface if CONFIG_FIRMWARE_MEMMAP is disabled
 */
#ifdef CONFIG_FIRMWARE_MEMMAP

int firmware_map_add_early(u64 start, u64 end, const char *type);
int firmware_map_add_hotplug(u64 start, u64 end, const char *type);
int firmware_map_remove(u64 start, u64 end, const char *type);

#else /* CONFIG_FIRMWARE_MEMMAP */

static inline int firmware_map_add_early(u64 start, u64 end, const char *type)
{
	return 0;
}

static inline int firmware_map_add_hotplug(u64 start, u64 end, const char *type)
{
	return 0;
}

static inline int firmware_map_remove(u64 start, u64 end, const char *type)
{
	return 0;
}

#endif /* CONFIG_FIRMWARE_MEMMAP */

#endif /* _freax_FIRMWARE_MAP_H */
