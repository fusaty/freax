/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef TOOLS_ARCH_ARC_UAPI_ASM_MMAN_FIX_H
#define TOOLS_ARCH_ARC_UAPI_ASM_MMAN_FIX_H
#include <uapi/asm-generic/mman.h>
/* MAP_32BIT is undefined on arc, fix it for perf */
#define MAP_32BIT	0
#endif
