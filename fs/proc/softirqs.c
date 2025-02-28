// SPDX-License-Identifier: GPL-2.0
#include <freax/init.h>
#include <freax/kernel_stat.h>
#include <freax/proc_fs.h>
#include <freax/seq_file.h>
#include "internal.h"

/*
 * /proc/softirqs  ... display the number of softirqs
 */
static int show_softirqs(struct seq_file *p, void *v)
{
	int i, j;

	seq_puts(p, "                    ");
	for_each_possible_cpu(i)
		seq_printf(p, "CPU%-8d", i);
	seq_putc(p, '\n');

	for (i = 0; i < NR_SOFTIRQS; i++) {
		seq_printf(p, "%12s:", softirq_to_name[i]);
		for_each_possible_cpu(j)
			seq_printf(p, " %10u", kstat_softirqs_cpu(i, j));
		seq_putc(p, '\n');
	}
	return 0;
}

static int __init proc_softirqs_init(void)
{
	struct proc_dir_entry *pde;

	pde = proc_create_single("softirqs", 0, NULL, show_softirqs);
	pde_make_permanent(pde);
	return 0;
}
fs_initcall(proc_softirqs_init);
