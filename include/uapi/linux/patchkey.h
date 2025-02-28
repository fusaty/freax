/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
/*
 * <freax/patchkey.h> -- definition of _PATCHKEY macro
 *
 * Copyright (C) 2005 Stuart Brady
 *
 * This exists because awe_voice.h defined its own _PATCHKEY and it wasn't
 * clear whether removing this would break anything in userspace.
 *
 * Do not include this file directly.  Please use <sys/soundcard.h> instead.
 * For kernel code, use <freax/soundcard.h>
 */

#ifndef _freax_PATCHKEY_H_INDIRECT
#error "patchkey.h included directly"
#endif

#ifndef _UAPI_freax_PATCHKEY_H
#define _UAPI_freax_PATCHKEY_H

/* Endian macros. */
#ifndef __KERNEL__
#  include <endian.h>
#endif

#if !defined(__KERNEL__)
#if defined(__BYTE_ORDER)
#  if __BYTE_ORDER == __BIG_ENDIAN
#    define _PATCHKEY(id) (0xfd00|id)
#  elif __BYTE_ORDER == __LITTLE_ENDIAN
#    define _PATCHKEY(id) ((id<<8)|0x00fd)
#  else
#    error "could not determine byte order"
#  endif
#endif
#endif

#endif /* _UAPI_freax_PATCHKEY_H */
