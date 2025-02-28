// SPDX-License-Identifier: GPL-2.0-only
/*
 * freax/lib/zlib_inflate/inflate_syms.c
 *
 * Exported symbols for the inflate functionality.
 *
 */

#include <freax/module.h>
#include <freax/init.h>

#include <freax/zlib.h>

EXPORT_SYMBOL(zlib_inflate_workspacesize);
EXPORT_SYMBOL(zlib_inflate);
EXPORT_SYMBOL(zlib_inflateInit2);
EXPORT_SYMBOL(zlib_inflateEnd);
EXPORT_SYMBOL(zlib_inflateReset);
EXPORT_SYMBOL(zlib_inflateIncomp); 
EXPORT_SYMBOL(zlib_inflate_blob);
MODULE_LICENSE("GPL");
