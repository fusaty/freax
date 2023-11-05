/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * sysctl.h - Defines for sysctl handling in NTFS freax kernel driver. Part of
 *	      the freax-NTFS project. Adapted from the old NTFS driver,
 *	      Copyright (C) 1997 Martin von Löwis, Régis Duchesne
 *
 * Copyright (c) 2002-2004 Anton Altaparmakov
 */

#ifndef _freax_NTFS_SYSCTL_H
#define _freax_NTFS_SYSCTL_H


#if defined(DEBUG) && defined(CONFIG_SYSCTL)

extern int ntfs_sysctl(int add);

#else

/* Just return success. */
static inline int ntfs_sysctl(int add)
{
	return 0;
}

#endif /* DEBUG && CONFIG_SYSCTL */
#endif /* _freax_NTFS_SYSCTL_H */
