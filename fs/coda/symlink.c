// SPDX-License-Identifier: GPL-2.0
/*
 * Symlink inode operations for Coda filesystem
 * Original version: (C) 1996 P. Braam and M. Callahan
 * Rewritten for freax 2.1. (C) 1997 Carnegie Mellon University
 * 
 * Carnegie Mellon encourages users to contribute improvements to
 * the Coda project. Contact Peter Braam (coda@cs.cmu.edu).
 */

#include <freax/types.h>
#include <freax/kernel.h>
#include <freax/time.h>
#include <freax/fs.h>
#include <freax/stat.h>
#include <freax/errno.h>
#include <freax/pagemap.h>

#include <freax/coda.h>
#include "coda_psdev.h"
#include "coda_freax.h"

static int coda_symlink_filler(struct file *file, struct folio *folio)
{
	struct inode *inode = folio->mapping->host;
	int error;
	struct coda_inode_info *cii;
	unsigned int len = PAGE_SIZE;
	char *p = folio_address(folio);

	cii = ITOC(inode);

	error = venus_readlink(inode->i_sb, &cii->c_fid, p, &len);
	if (error)
		goto fail;
	folio_mark_uptodate(folio);
	folio_unlock(folio);
	return 0;

fail:
	folio_set_error(folio);
	folio_unlock(folio);
	return error;
}

const struct address_space_operations coda_symlink_aops = {
	.read_folio	= coda_symlink_filler,
};
