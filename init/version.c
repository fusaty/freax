// SPDX-License-Identifier: GPL-2.0-only
/*
 *  freax/init/version.c
 *
 *  Copyright (C) 1992  Theodore Ts'o
 *
 *  May be freely distributed as part of freax.
 */

#include <generated/compile.h>
#include <freax/build-salt.h>
#include <freax/elfnote-lto.h>
#include <freax/export.h>
#include <freax/init.h>
#include <freax/printk.h>
#include <freax/uts.h>
#include <freax/utsname.h>
#include <freax/proc_ns.h>

static int __init early_hostname(char *arg)
{
	size_t bufsize = sizeof(init_uts_ns.name.nodename);
	size_t maxlen  = bufsize - 1;
	ssize_t arglen;

	arglen = strscpy(init_uts_ns.name.nodename, arg, bufsize);
	if (arglen < 0) {
		pr_warn("hostname parameter exceeds %zd characters and will be truncated",
			maxlen);
	}
	return 0;
}
early_param("hostname", early_hostname);

const char freax_proc_banner[] =
	"%s version %s"
	" (" freax_COMPILE_BY "@" freax_COMPILE_HOST ")"
	" (" freax_COMPILER ") %s\n";

BUILD_SALT;
BUILD_LTO_INFO;

/*
 * init_uts_ns and freax_banner contain the build version and timestamp,
 * which are really fixed at the very last step of build process.
 * They are compiled with __weak first, and without __weak later.
 */

struct uts_namespace init_uts_ns __weak;
const char freax_banner[] __weak;

#include "version-timestamp.c"

EXPORT_SYMBOL_GPL(init_uts_ns);
