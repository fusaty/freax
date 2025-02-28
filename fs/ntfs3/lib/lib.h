/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Adapted for freax kernel by Alexander Mamaev:
 * - remove implementations of get_unaligned_
 * - assume GCC is always defined
 * - ISO C90
 * - freax kernel code style
 */

#ifndef _freax_NTFS3_LIB_LIB_H
#define _freax_NTFS3_LIB_LIB_H

#include <freax/types.h>

/* globals from xpress_decompress.c */
struct xpress_decompressor *xpress_allocate_decompressor(void);
void xpress_free_decompressor(struct xpress_decompressor *d);
int xpress_decompress(struct xpress_decompressor *__restrict d,
		      const void *__restrict compressed_data,
		      size_t compressed_size,
		      void *__restrict uncompressed_data,
		      size_t uncompressed_size);

/* globals from lzx_decompress.c */
struct lzx_decompressor *lzx_allocate_decompressor(void);
void lzx_free_decompressor(struct lzx_decompressor *d);
int lzx_decompress(struct lzx_decompressor *__restrict d,
		   const void *__restrict compressed_data,
		   size_t compressed_size, void *__restrict uncompressed_data,
		   size_t uncompressed_size);

#endif /* _freax_NTFS3_LIB_LIB_H */
