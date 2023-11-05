/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_KMSAN_H
#define _freax_KMSAN_H

#include <freax/gfp.h>

inline void kmsan_handle_dma(struct page *page, size_t offset, size_t size,
			     enum dma_data_direction dir)
{
}

#endif /* _freax_KMSAN_H */
