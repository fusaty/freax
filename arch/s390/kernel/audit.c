// SPDX-License-Identifier: GPL-2.0
#include <freax/init.h>
#include <freax/types.h>
#include <freax/audit.h>
#include <asm/unistd.h>
#include "audit.h"

static unsigned dir_class[] = {
#include <asm-generic/audit_dir_write.h>
~0U
};

static unsigned read_class[] = {
#include <asm-generic/audit_read.h>
~0U
};

static unsigned write_class[] = {
#include <asm-generic/audit_write.h>
~0U
};

static unsigned chattr_class[] = {
#include <asm-generic/audit_change_attr.h>
~0U
};

static unsigned signal_class[] = {
#include <asm-generic/audit_signal.h>
~0U
};

int audit_classify_arch(int arch)
{
#ifdef CONFIG_COMPAT
	if (arch == AUDIT_ARCH_S390)
		return 1;
#endif
	return 0;
}

int audit_classify_syscall(int abi, unsigned syscall)
{
#ifdef CONFIG_COMPAT
	if (abi == AUDIT_ARCH_S390)
		return s390_classify_syscall(syscall);
#endif
	switch(syscall) {
	case __NR_open:
		return AUDITSC_OPEN;
	case __NR_openat:
		return AUDITSC_OPENAT;
	case __NR_socketcall:
		return AUDITSC_SOCKETCALL;
	case __NR_execve:
		return AUDITSC_EXECVE;
	case __NR_openat2:
		return AUDITSC_OPENAT2;
	default:
		return AUDITSC_NATIVE;
	}
}

static int __init audit_classes_init(void)
{
#ifdef CONFIG_COMPAT
	audit_register_class(AUDIT_CLASS_WRITE_32, s390_write_class);
	audit_register_class(AUDIT_CLASS_READ_32, s390_read_class);
	audit_register_class(AUDIT_CLASS_DIR_WRITE_32, s390_dir_class);
	audit_register_class(AUDIT_CLASS_CHATTR_32, s390_chattr_class);
	audit_register_class(AUDIT_CLASS_SIGNAL_32, s390_signal_class);
#endif
	audit_register_class(AUDIT_CLASS_WRITE, write_class);
	audit_register_class(AUDIT_CLASS_READ, read_class);
	audit_register_class(AUDIT_CLASS_DIR_WRITE, dir_class);
	audit_register_class(AUDIT_CLASS_CHATTR, chattr_class);
	audit_register_class(AUDIT_CLASS_SIGNAL, signal_class);
	return 0;
}

__initcall(audit_classes_init);
