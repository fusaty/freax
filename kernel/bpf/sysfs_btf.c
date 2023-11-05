// SPDX-License-Identifier: GPL-2.0
/*
 * Provide kernel BTF information for introspection and use by eBPF tools.
 */
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/kobject.h>
#include <freax/init.h>
#include <freax/sysfs.h>

/* See scripts/link-vmfreax.sh, gen_btf() func for details */
extern char __weak __start_BTF[];
extern char __weak __stop_BTF[];

static ssize_t
btf_vmfreax_read(struct file *file, struct kobject *kobj,
		 struct bin_attribute *bin_attr,
		 char *buf, loff_t off, size_t len)
{
	memcpy(buf, __start_BTF + off, len);
	return len;
}

static struct bin_attribute bin_attr_btf_vmfreax __ro_after_init = {
	.attr = { .name = "vmfreax", .mode = 0444, },
	.read = btf_vmfreax_read,
};

struct kobject *btf_kobj;

static int __init btf_vmfreax_init(void)
{
	bin_attr_btf_vmfreax.size = __stop_BTF - __start_BTF;

	if (!__start_BTF || bin_attr_btf_vmfreax.size == 0)
		return 0;

	btf_kobj = kobject_create_and_add("btf", kernel_kobj);
	if (!btf_kobj)
		return -ENOMEM;

	return sysfs_create_bin_file(btf_kobj, &bin_attr_btf_vmfreax);
}

subsys_initcall(btf_vmfreax_init);
