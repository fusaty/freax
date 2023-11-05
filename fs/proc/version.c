// SPDX-License-Identifier: GPL-2.0
#include <freax/fs.h>
#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/proc_fs.h>
#include <freax/seq_file.h>
#include <freax/utsname.h>
#include "internal.h"

static int version_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, freax_proc_banner,
		utsname()->sysname,
		utsname()->release,
		utsname()->version);
	return 0;
}

static int __init proc_version_init(void)
{
	struct proc_dir_entry *pde;

	pde = proc_create_single("version", 0, NULL, version_proc_show);
	pde_make_permanent(pde);
	return 0;
}
fs_initcall(proc_version_init);
