// SPDX-License-Identifier: LGPL-2.1
#ifndef __TOOLS_freax_ZALLOC_H
#define __TOOLS_freax_ZALLOC_H

#include <stddef.h>

void *zalloc(size_t size);
void __zfree(void **ptr);

#define zfree(ptr) __zfree((void **)(ptr))

#endif // __TOOLS_freax_ZALLOC_H
