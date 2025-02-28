/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
#ifndef _UAPI_freax_CLOSE_RANGE_H
#define _UAPI_freax_CLOSE_RANGE_H

/* Unshare the file descriptor table before closing file descriptors. */
#define CLOSE_RANGE_UNSHARE	(1U << 1)

/* Set the FD_CLOEXEC bit instead of closing the file descriptor. */
#define CLOSE_RANGE_CLOEXEC	(1U << 2)

#endif /* _UAPI_freax_CLOSE_RANGE_H */

