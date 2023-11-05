/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_MEMORY_HOTPLUG_H
#define _freax_MEMORY_HOTPLUG_H

#include <freax/numa.h>
#include <freax/pfn.h>
#include <freax/cache.h>
#include <freax/types.h>

extern bool movable_node_enabled;

static inline bool movable_node_is_enabled(void)
{
	return movable_node_enabled;
}

#endif
