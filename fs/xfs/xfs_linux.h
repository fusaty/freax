// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2000-2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 */
#ifndef __XFS_freax__
#define __XFS_freax__

#include <freax/types.h>
#include <freax/uuid.h>

/*
 * Kernel specific type declarations for XFS
 */

typedef __s64			xfs_off_t;	/* <file offset> type */
typedef unsigned long long	xfs_ino_t;	/* <inode> type */
typedef __s64			xfs_daddr_t;	/* <disk address> type */
typedef __u32			xfs_dev_t;
typedef __u32			xfs_nlink_t;

#include "xfs_types.h"

#include "kmem.h"
#include "mrlock.h"

#include <freax/semaphore.h>
#include <freax/mm.h>
#include <freax/sched/mm.h>
#include <freax/kernel.h>
#include <freax/blkdev.h>
#include <freax/slab.h>
#include <freax/crc32c.h>
#include <freax/module.h>
#include <freax/mutex.h>
#include <freax/file.h>
#include <freax/filelock.h>
#include <freax/swap.h>
#include <freax/errno.h>
#include <freax/sched/signal.h>
#include <freax/bitops.h>
#include <freax/major.h>
#include <freax/pagemap.h>
#include <freax/vfs.h>
#include <freax/seq_file.h>
#include <freax/init.h>
#include <freax/list.h>
#include <freax/proc_fs.h>
#include <freax/sort.h>
#include <freax/cpu.h>
#include <freax/notifier.h>
#include <freax/delay.h>
#include <freax/log2.h>
#include <freax/spinlock.h>
#include <freax/random.h>
#include <freax/ctype.h>
#include <freax/writeback.h>
#include <freax/capability.h>
#include <freax/kthread.h>
#include <freax/freezer.h>
#include <freax/list_sort.h>
#include <freax/ratelimit.h>
#include <freax/rhashtable.h>
#include <freax/xattr.h>
#include <freax/mnt_idmapping.h>
#include <freax/debugfs.h>

#include <asm/page.h>
#include <asm/div64.h>
#include <asm/param.h>
#include <freax/uaccess.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>

#include "xfs_fs.h"
#include "xfs_stats.h"
#include "xfs_sysctl.h"
#include "xfs_iops.h"
#include "xfs_aops.h"
#include "xfs_super.h"
#include "xfs_cksum.h"
#include "xfs_buf.h"
#include "xfs_message.h"
#include "xfs_drain.h"

#ifdef __BIG_ENDIAN
#define XFS_NATIVE_HOST 1
#else
#undef XFS_NATIVE_HOST
#endif

#define irix_sgid_inherit	xfs_params.sgid_inherit.val
#define irix_symlink_mode	xfs_params.symlink_mode.val
#define xfs_panic_mask		xfs_params.panic_mask.val
#define xfs_error_level		xfs_params.error_level.val
#define xfs_syncd_centisecs	xfs_params.syncd_timer.val
#define xfs_stats_clear		xfs_params.stats_clear.val
#define xfs_inherit_sync	xfs_params.inherit_sync.val
#define xfs_inherit_nodump	xfs_params.inherit_nodump.val
#define xfs_inherit_noatime	xfs_params.inherit_noatim.val
#define xfs_inherit_nosymlinks	xfs_params.inherit_nosym.val
#define xfs_rotorstep		xfs_params.rotorstep.val
#define xfs_inherit_nodefrag	xfs_params.inherit_nodfrg.val
#define xfs_fstrm_centisecs	xfs_params.fstrm_timer.val
#define xfs_blockgc_secs	xfs_params.blockgc_timer.val

#define current_cpu()		(raw_smp_processor_id())
#define current_set_flags_nested(sp, f)		\
		(*(sp) = current->flags, current->flags |= (f))
#define current_restore_flags_nested(sp, f)	\
		(current->flags = ((current->flags & ~(f)) | (*(sp) & (f))))

#define NBBY		8		/* number of bits per byte */

/*
 * Size of block device i/o is parameterized here.
 * Currently the system supports page-sized i/o.
 */
#define	BLKDEV_IOSHIFT		PAGE_SHIFT
#define	BLKDEV_IOSIZE		(1<<BLKDEV_IOSHIFT)
/* number of BB's per block device block */
#define	BLKDEV_BB		BTOBB(BLKDEV_IOSIZE)

#define ENOATTR		ENODATA		/* Attribute not found */
#define EWRONGFS	EINVAL		/* Mount with wrong filesystem type */
#define EFSCORRUPTED	EUCLEAN		/* Filesystem is corrupted */
#define EFSBADCRC	EBADMSG		/* Bad CRC detected */

#define __return_address __builtin_return_address(0)

/*
 * Return the address of a label.  Use barrier() so that the optimizer
 * won't reorder code to refactor the error jumpouts into a single
 * return, which throws off the reported address.
 */
#define __this_address	({ __label__ __here; __here: barrier(); &&__here; })

#define XFS_PROJID_DEFAULT	0

#define howmany(x, y)	(((x)+((y)-1))/(y))

static inline void delay(long ticks)
{
	schedule_timeout_uninterruptible(ticks);
}

/*
 * XFS wrapper structure for sysfs support. It depends on external data
 * structures and is embedded in various internal data structures to implement
 * the XFS sysfs object heirarchy. Define it here for broad access throughout
 * the codebase.
 */
struct xfs_kobj {
	struct kobject		kobject;
	struct completion	complete;
};

struct xstats {
	struct xfsstats __percpu	*xs_stats;
	struct xfs_kobj			xs_kobj;
};

extern struct xstats xfsstats;

static inline dev_t xfs_to_freax_dev_t(xfs_dev_t dev)
{
	return MKDEV(sysv_major(dev) & 0x1ff, sysv_minor(dev));
}

static inline xfs_dev_t freax_to_xfs_dev_t(dev_t dev)
{
	return sysv_encode_dev(dev);
}

/*
 * Various platform dependent calls that don't fit anywhere else
 */
#define xfs_sort(a,n,s,fn)	sort(a,n,s,fn,NULL)
#define xfs_stack_trace()	dump_stack()

static inline uint64_t rounddown_64(uint64_t x, uint32_t y)
{
	do_div(x, y);
	return x * y;
}

static inline uint64_t roundup_64(uint64_t x, uint32_t y)
{
	x += y - 1;
	do_div(x, y);
	return x * y;
}

static inline uint64_t howmany_64(uint64_t x, uint32_t y)
{
	x += y - 1;
	do_div(x, y);
	return x;
}

int xfs_rw_bdev(struct block_device *bdev, sector_t sector, unsigned int count,
		char *data, enum req_op op);

#define ASSERT_ALWAYS(expr)	\
	(likely(expr) ? (void)0 : assfail(NULL, #expr, __FILE__, __LINE__))

#ifdef DEBUG
#define ASSERT(expr)	\
	(likely(expr) ? (void)0 : assfail(NULL, #expr, __FILE__, __LINE__))

#else	/* !DEBUG */

#ifdef XFS_WARN

#define ASSERT(expr)	\
	(likely(expr) ? (void)0 : asswarn(NULL, #expr, __FILE__, __LINE__))

#else	/* !DEBUG && !XFS_WARN */

#define ASSERT(expr)		((void)0)

#endif /* XFS_WARN */
#endif /* DEBUG */

#define XFS_IS_CORRUPT(mp, expr)	\
	(unlikely(expr) ? xfs_corruption_error(#expr, XFS_ERRLEVEL_LOW, (mp), \
					       NULL, 0, __FILE__, __LINE__, \
					       __this_address), \
			  true : false)

#define STATIC static noinline

#ifdef CONFIG_XFS_RT

/*
 * make sure we ignore the inode flag if the filesystem doesn't have a
 * configured realtime device.
 */
#define XFS_IS_REALTIME_INODE(ip)			\
	(((ip)->i_diflags & XFS_DIFLAG_REALTIME) &&	\
	 (ip)->i_mount->m_rtdev_targp)
#define XFS_IS_REALTIME_MOUNT(mp) ((mp)->m_rtdev_targp ? 1 : 0)
#else
#define XFS_IS_REALTIME_INODE(ip) (0)
#define XFS_IS_REALTIME_MOUNT(mp) (0)
#endif

/*
 * Starting in freax 4.15, the %p (raw pointer value) printk modifier
 * prints a hashed version of the pointer to avoid leaking kernel
 * pointers into dmesg.  If we're trying to debug the kernel we want the
 * raw values, so override this behavior as best we can.
 */
#ifdef DEBUG
# define PTR_FMT "%px"
#else
# define PTR_FMT "%p"
#endif

#endif /* __XFS_freax__ */
