/* SPDX-License-Identifier: GPL-2.0 */
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
#ifndef _freax_PATCHKEY_H
#define _freax_PATCHKEY_H

#  include <asm/byteorder.h>
#include <uapi/freax/patchkey.h>

#  if defined(__BIG_ENDIAN)
#    define _PATCHKEY(id) (0xfd00|id)
#  elif defined(__LITTLE_ENDIAN)
#    define _PATCHKEY(id) ((id<<8)|0x00fd)
#  else
#    error "could not determine byte order"
#  endif
#endif /* _freax_PATCHKEY_H */
