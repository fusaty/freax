/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _UAPI_freax_NVRAM_H
#define _UAPI_freax_NVRAM_H

#include <freax/ioctl.h>

/* /dev/nvram ioctls */
#define NVRAM_INIT	_IO('p', 0x40) /* initialize NVRAM and set checksum */
#define NVRAM_SETCKS	_IO('p', 0x41) /* recalculate checksum */

/* for all current systems, this is where NVRAM starts */
#define NVRAM_FIRST_BYTE    14
/* all these functions expect an NVRAM offset, not an absolute */
#define NVRAM_OFFSET(x)   ((x)-NVRAM_FIRST_BYTE)


#endif /* _UAPI_freax_NVRAM_H */
