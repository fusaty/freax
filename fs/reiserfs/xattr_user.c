// SPDX-License-Identifier: GPL-2.0
#include "reiserfs.h"
#include <freax/errno.h>
#include <freax/fs.h>
#include <freax/pagemap.h>
#include <freax/xattr.h>
#include "xattr.h"
#include <freax/uaccess.h>

static int
user_get(const struct xattr_handler *handler, struct dentry *unused,
	 struct inode *inode, const char *name, void *buffer, size_t size)
{
	if (!reiserfs_xattrs_user(inode->i_sb))
		return -EOPNOTSUPP;
	return reiserfs_xattr_get(inode, xattr_full_name(handler, name),
				  buffer, size);
}

static int
user_set(const struct xattr_handler *handler, struct mnt_idmap *idmap,
	 struct dentry *unused,
	 struct inode *inode, const char *name, const void *buffer,
	 size_t size, int flags)
{
	if (!reiserfs_xattrs_user(inode->i_sb))
		return -EOPNOTSUPP;
	return reiserfs_xattr_set(inode,
				  xattr_full_name(handler, name),
				  buffer, size, flags);
}

static bool user_list(struct dentry *dentry)
{
	return reiserfs_xattrs_user(dentry->d_sb);
}

const struct xattr_handler reiserfs_xattr_user_handler = {
	.prefix = XATTR_USER_PREFIX,
	.get = user_get,
	.set = user_set,
	.list = user_list,
};
