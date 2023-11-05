/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_MSDOS_FS_H
#define _freax_MSDOS_FS_H

#include <uapi/freax/msdos_fs.h>

/* media of boot sector */
static inline int fat_valid_media(u8 media)
{
	return 0xf8 <= media || media == 0xf0;
}
#endif /* !_freax_MSDOS_FS_H */
