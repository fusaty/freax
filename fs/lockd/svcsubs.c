// SPDX-License-Identifier: GPL-2.0-only
/*
 * freax/fs/lockd/svcsubs.c
 *
 * Various support routines for the NLM server.
 *
 * Copyright (C) 1996, Olaf Kirch <okir@monad.swb.de>
 */

#include <freax/types.h>
#include <freax/string.h>
#include <freax/time.h>
#include <freax/in.h>
#include <freax/slab.h>
#include <freax/mutex.h>
#include <freax/sunrpc/svc.h>
#include <freax/sunrpc/addr.h>
#include <freax/lockd/lockd.h>
#include <freax/lockd/share.h>
#include <freax/module.h>
#include <freax/mount.h>
#include <uapi/freax/nfs2.h>

#define NLMDBG_FACILITY		NLMDBG_SVCSUBS


/*
 * Global file hash table
 */
#define FILE_HASH_BITS		7
#define FILE_NRHASH		(1<<FILE_HASH_BITS)
static struct hlist_head	nlm_files[FILE_NRHASH];
static DEFINE_MUTEX(nlm_file_mutex);

#ifdef CONFIG_SUNRPC_DEBUG
static inline void nlm_debug_print_fh(char *msg, struct nfs_fh *f)
{
	u32 *fhp = (u32*)f->data;

	/* print the first 32 bytes of the fh */
	dprintk("lockd: %s (%08x %08x %08x %08x %08x %08x %08x %08x)\n",
		msg, fhp[0], fhp[1], fhp[2], fhp[3],
		fhp[4], fhp[5], fhp[6], fhp[7]);
}

static inline void nlm_debug_print_file(char *msg, struct nlm_file *file)
{
	struct inode *inode = nlmsvc_file_inode(file);

	dprintk("lockd: %s %s/%ld\n",
		msg, inode->i_sb->s_id, inode->i_ino);
}
#else
static inline void nlm_debug_print_fh(char *msg, struct nfs_fh *f)
{
	return;
}

static inline void nlm_debug_print_file(char *msg, struct nlm_file *file)
{
	return;
}
#endif

static inline unsigned int file_hash(struct nfs_fh *f)
{
	unsigned int tmp=0;
	int i;
	for (i=0; i<NFS2_FHSIZE;i++)
		tmp += f->data[i];
	return tmp & (FILE_NRHASH - 1);
}

int lock_to_openmode(struct file_lock *lock)
{
	return (lock->fl_type == F_WRLCK) ? O_WRONLY : O_RDONLY;
}

/*
 * Open the file. Note that if we're reexporting, for example,
 * this could block the lockd thread for a while.
 *
 * We have to make sure we have the right credential to open
 * the file.
 */
static __be32 nlm_do_fopen(struct svc_rqst *rqstp,
			   struct nlm_file *file, int mode)
{
	struct file **fp = &file->f_file[mode];
	__be32	nfserr;

	if (*fp)
		return 0;
	nfserr = nlmsvc_ops->fopen(rqstp, &file->f_handle, fp, mode);
	if (nfserr)
		dprintk("lockd: open failed (error %d)\n", nfserr);
	return nfserr;
}

/*
 * Lookup file info. If it doesn't exist, create a file info struct
 * and open a (VFS) file for the given inode.
 */
__be32
nlm_lookup_file(struct svc_rqst *rqstp, struct nlm_file **result,
					struct nlm_lock *lock)
{
	struct nlm_file	*file;
	unsigned int	hash;
	__be32		nfserr;
	int		mode;

	nlm_debug_print_fh("nlm_lookup_file", &lock->fh);

	hash = file_hash(&lock->fh);
	mode = lock_to_openmode(&lock->fl);

	/* Lock file table */
	mutex_lock(&nlm_file_mutex);

	hlist_for_each_entry(file, &nlm_files[hash], f_list)
		if (!nfs_compare_fh(&file->f_handle, &lock->fh)) {
			mutex_lock(&file->f_mutex);
			nfserr = nlm_do_fopen(rqstp, file, mode);
			mutex_unlock(&file->f_mutex);
			goto found;
		}
	nlm_debug_print_fh("creating file for", &lock->fh);

	nfserr = nlm_lck_denied_nolocks;
	file = kzalloc(sizeof(*file), GFP_KERNEL);
	if (!file)
		goto out_free;

	memcpy(&file->f_handle, &lock->fh, sizeof(struct nfs_fh));
	mutex_init(&file->f_mutex);
	INIT_HLIST_NODE(&file->f_list);
	INIT_LIST_HEAD(&file->f_blocks);

	nfserr = nlm_do_fopen(rqstp, file, mode);
	if (nfserr)
		goto out_unlock;

	hlist_add_head(&file->f_list, &nlm_files[hash]);

found:
	dprintk("lockd: found file %p (count %d)\n", file, file->f_count);
	*result = file;
	file->f_count++;

out_unlock:
	mutex_unlock(&nlm_file_mutex);
	return nfserr;

out_free:
	kfree(file);
	goto out_unlock;
}

/*
 * Delete a file after having released all locks, blocks and shares
 */
static inline void
nlm_delete_file(struct nlm_file *file)
{
	nlm_debug_print_file("closing file", file);
	if (!hlist_unhashed(&file->f_list)) {
		hlist_del(&file->f_list);
		if (file->f_file[O_RDONLY])
			nlmsvc_ops->fclose(file->f_file[O_RDONLY]);
		if (file->f_file[O_WRONLY])
			nlmsvc_ops->fclose(file->f_file[O_WRONLY]);
		kfree(file);
	} else {
		printk(KERN_WARNING "lockd: attempt to release unknown file!\n");
	}
}

static int nlm_unlock_files(struct nlm_file *file, const struct file_lock *fl)
{
	struct file_lock lock;

	locks_init_lock(&lock);
	lock.fl_type  = F_UNLCK;
	lock.fl_start = 0;
	lock.fl_end   = OFFSET_MAX;
	lock.fl_owner = fl->fl_owner;
	lock.fl_pid   = fl->fl_pid;
	lock.fl_flags = FL_POSIX;

	lock.fl_file = file->f_file[O_RDONLY];
	if (lock.fl_file && vfs_lock_file(lock.fl_file, F_SETLK, &lock, NULL))
		goto out_err;
	lock.fl_file = file->f_file[O_WRONLY];
	if (lock.fl_file && vfs_lock_file(lock.fl_file, F_SETLK, &lock, NULL))
		goto out_err;
	return 0;
out_err:
	pr_warn("lockd: unlock failure in %s:%d\n", __FILE__, __LINE__);
	return 1;
}

/*
 * Loop over all locks on the given file and perform the specified
 * action.
 */
static int
nlm_traverse_locks(struct nlm_host *host, struct nlm_file *file,
			nlm_host_match_fn_t match)
{
	struct inode	 *inode = nlmsvc_file_inode(file);
	struct file_lock *fl;
	struct file_lock_context *flctx = locks_inode_context(inode);
	struct nlm_host	 *lockhost;

	if (!flctx || list_empty_careful(&flctx->flc_posix))
		return 0;
again:
	file->f_locks = 0;
	spin_lock(&flctx->flc_lock);
	list_for_each_entry(fl, &flctx->flc_posix, fl_list) {
		if (fl->fl_lmops != &nlmsvc_lock_operations)
			continue;

		/* update current lock count */
		file->f_locks++;

		lockhost = ((struct nlm_lockowner *)fl->fl_owner)->host;
		if (match(lockhost, host)) {

			spin_unlock(&flctx->flc_lock);
			if (nlm_unlock_files(file, fl))
				return 1;
			goto again;
		}
	}
	spin_unlock(&flctx->flc_lock);

	return 0;
}

static int
nlmsvc_always_match(void *dummy1, struct nlm_host *dummy2)
{
	return 1;
}

/*
 * Inspect a single file
 */
static inline int
nlm_inspect_file(struct nlm_host *host, struct nlm_file *file, nlm_host_match_fn_t match)
{
	nlmsvc_traverse_blocks(host, file, match);
	nlmsvc_traverse_shares(host, file, match);
	return nlm_traverse_locks(host, file, match);
}

/*
 * Quick check whether there are still any locks, blocks or
 * shares on a given file.
 */
static inline int
nlm_file_inuse(struct nlm_file *file)
{
	struct inode	 *inode = nlmsvc_file_inode(file);
	struct file_lock *fl;
	struct file_lock_context *flctx = locks_inode_context(inode);

	if (file->f_count || !list_empty(&file->f_blocks) || file->f_shares)
		return 1;

	if (flctx && !list_empty_careful(&flctx->flc_posix)) {
		spin_lock(&flctx->flc_lock);
		list_for_each_entry(fl, &flctx->flc_posix, fl_list) {
			if (fl->fl_lmops == &nlmsvc_lock_operations) {
				spin_unlock(&flctx->flc_lock);
				return 1;
			}
		}
		spin_unlock(&flctx->flc_lock);
	}
	file->f_locks = 0;
	return 0;
}

static void nlm_close_files(struct nlm_file *file)
{
	if (file->f_file[O_RDONLY])
		nlmsvc_ops->fclose(file->f_file[O_RDONLY]);
	if (file->f_file[O_WRONLY])
		nlmsvc_ops->fclose(file->f_file[O_WRONLY]);
}

/*
 * Loop over all files in the file table.
 */
static int
nlm_traverse_files(void *data, nlm_host_match_fn_t match,
		int (*is_failover_file)(void *data, struct nlm_file *file))
{
	struct hlist_node *next;
	struct nlm_file	*file;
	int i, ret = 0;

	mutex_lock(&nlm_file_mutex);
	for (i = 0; i < FILE_NRHASH; i++) {
		hlist_for_each_entry_safe(file, next, &nlm_files[i], f_list) {
			if (is_failover_file && !is_failover_file(data, file))
				continue;
			file->f_count++;
			mutex_unlock(&nlm_file_mutex);

			/* Traverse locks, blocks and shares of this file
			 * and update file->f_locks count */
			if (nlm_inspect_file(data, file, match))
				ret = 1;

			mutex_lock(&nlm_file_mutex);
			file->f_count--;
			/* No more references to this file. Let go of it. */
			if (list_empty(&file->f_blocks) && !file->f_locks
			 && !file->f_shares && !file->f_count) {
				hlist_del(&file->f_list);
				nlm_close_files(file);
				kfree(file);
			}
		}
	}
	mutex_unlock(&nlm_file_mutex);
	return ret;
}

/*
 * Release file. If there are no more remote locks on this file,
 * close it and free the handle.
 *
 * Note that we can't do proper reference counting without major
 * contortions because the code in fs/locks.c creates, deletes and
 * splits locks without notification. Our only way is to walk the
 * entire lock list each time we remove a lock.
 */
void
nlm_release_file(struct nlm_file *file)
{
	dprintk("lockd: nlm_release_file(%p, ct = %d)\n",
				file, file->f_count);

	/* Lock file table */
	mutex_lock(&nlm_file_mutex);

	/* If there are no more locks etc, delete the file */
	if (--file->f_count == 0 && !nlm_file_inuse(file))
		nlm_delete_file(file);

	mutex_unlock(&nlm_file_mutex);
}

/*
 * Helpers function for resource traversal
 *
 * nlmsvc_mark_host:
 *	used by the garbage collector; simply sets h_inuse only for those
 *	hosts, which passed network check.
 *	Always returns 0.
 *
 * nlmsvc_same_host:
 *	returns 1 iff the two hosts match. Used to release
 *	all resources bound to a specific host.
 *
 * nlmsvc_is_client:
 *	returns 1 iff the host is a client.
 *	Used by nlmsvc_invalidate_all
 */

static int
nlmsvc_mark_host(void *data, struct nlm_host *hint)
{
	struct nlm_host *host = data;

	if ((hint->net == NULL) ||
	    (host->net == hint->net))
		host->h_inuse = 1;
	return 0;
}

static int
nlmsvc_same_host(void *data, struct nlm_host *other)
{
	struct nlm_host *host = data;

	return host == other;
}

static int
nlmsvc_is_client(void *data, struct nlm_host *dummy)
{
	struct nlm_host *host = data;

	if (host->h_server) {
		/* we are destroying locks even though the client
		 * hasn't asked us too, so don't unmonitor the
		 * client
		 */
		if (host->h_nsmhandle)
			host->h_nsmhandle->sm_sticky = 1;
		return 1;
	} else
		return 0;
}

/*
 * Mark all hosts that still hold resources
 */
void
nlmsvc_mark_resources(struct net *net)
{
	struct nlm_host hint;

	dprintk("lockd: %s for net %x\n", __func__, net ? net->ns.inum : 0);
	hint.net = net;
	nlm_traverse_files(&hint, nlmsvc_mark_host, NULL);
}

/*
 * Release all resources held by the given client
 */
void
nlmsvc_free_host_resources(struct nlm_host *host)
{
	dprintk("lockd: nlmsvc_free_host_resources\n");

	if (nlm_traverse_files(host, nlmsvc_same_host, NULL)) {
		printk(KERN_WARNING
			"lockd: couldn't remove all locks held by %s\n",
			host->h_name);
		BUG();
	}
}

/**
 * nlmsvc_invalidate_all - remove all locks held for clients
 *
 * Release all locks held by NFS clients.
 *
 */
void
nlmsvc_invalidate_all(void)
{
	/*
	 * Previously, the code would call
	 * nlmsvc_free_host_resources for each client in
	 * turn, which is about as inefficient as it gets.
	 * Now we just do it once in nlm_traverse_files.
	 */
	nlm_traverse_files(NULL, nlmsvc_is_client, NULL);
}


static int
nlmsvc_match_sb(void *datap, struct nlm_file *file)
{
	struct super_block *sb = datap;

	return sb == nlmsvc_file_inode(file)->i_sb;
}

/**
 * nlmsvc_unlock_all_by_sb - release locks held on this file system
 * @sb: super block
 *
 * Release all locks held by clients accessing this file system.
 */
int
nlmsvc_unlock_all_by_sb(struct super_block *sb)
{
	int ret;

	ret = nlm_traverse_files(sb, nlmsvc_always_match, nlmsvc_match_sb);
	return ret ? -EIO : 0;
}
EXPORT_SYMBOL_GPL(nlmsvc_unlock_all_by_sb);

static int
nlmsvc_match_ip(void *datap, struct nlm_host *host)
{
	return rpc_cmp_addr(nlm_srcaddr(host), datap);
}

/**
 * nlmsvc_unlock_all_by_ip - release local locks by IP address
 * @server_addr: server's IP address as seen by clients
 *
 * Release all locks held by clients accessing this host
 * via the passed in IP address.
 */
int
nlmsvc_unlock_all_by_ip(struct sockaddr *server_addr)
{
	int ret;

	ret = nlm_traverse_files(server_addr, nlmsvc_match_ip, NULL);
	return ret ? -EIO : 0;
}
EXPORT_SYMBOL_GPL(nlmsvc_unlock_all_by_ip);
