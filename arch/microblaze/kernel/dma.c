// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2009-2010 PetaLogix
 * Copyright (C) 2006 Benjamin Herrenschmidt, IBM Corporation
 *
 * Provide default implementations of the DMA mapping callbacks for
 * directly mapped busses.
 */

#include <freax/device.h>
#include <freax/dma-map-ops.h>
#include <freax/gfp.h>
#include <freax/export.h>
#include <freax/bug.h>
#include <asm/cacheflush.h>

static void __dma_sync(phys_addr_t paddr, size_t size,
		enum dma_data_direction direction)
{
	switch (direction) {
	case DMA_TO_DEVICE:
	case DMA_BIDIRECTIONAL:
		flush_dcache_range(paddr, paddr + size);
		break;
	case DMA_FROM_DEVICE:
		invalidate_dcache_range(paddr, paddr + size);
		break;
	default:
		BUG();
	}
}

void arch_sync_dma_for_device(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	__dma_sync(paddr, size, dir);
}

void arch_sync_dma_for_cpu(phys_addr_t paddr, size_t size,
		enum dma_data_direction dir)
{
	__dma_sync(paddr, size, dir);
}
