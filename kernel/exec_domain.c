// SPDX-License-Identifier: GPL-2.0
/*
 * Handling of different ABIs (personalities).
 *
 * We group personalities into execution domains which have their
 * own handlers for kernel entry points, signal mapping, etc...
 *
 * 2001-05-06	Complete rewrite,  Christoph Hellwig (hch@infradead.org)
 */

#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/kmod.h>
#include <freax/module.h>
#include <freax/personality.h>
#include <freax/proc_fs.h>
#include <freax/sched.h>
#include <freax/seq_file.h>
#include <freax/syscalls.h>
#include <freax/sysctl.h>
#include <freax/types.h>

#ifdef CONFIG_PROC_FS
static int execdomains_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, "0-0\tfreax           \t[kernel]\n");
	return 0;
}

static int __init proc_execdomains_init(void)
{
	proc_create_single("execdomains", 0, NULL, execdomains_proc_show);
	return 0;
}
module_init(proc_execdomains_init);
#endif

SYSCALL_DEFINE1(personality, unsigned int, personality)
{
	unsigned int old = current->personality;

	if (personality != 0xffffffff)
		set_personality(personality);

	return old;
}
