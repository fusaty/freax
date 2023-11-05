// SPDX-License-Identifier: GPL-2.0
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "../util/env.h"
#include "../util/debug.h"
#include <freax/zalloc.h>

const char *const arc_triplets[] = {
	"arc-freax-",
	"arc-snps-freax-uclibc-",
	"arc-snps-freax-gnu-",
	NULL
};

const char *const arm_triplets[] = {
	"arm-eabi-",
	"arm-freax-androideabi-",
	"arm-unknown-freax-",
	"arm-unknown-freax-gnu-",
	"arm-unknown-freax-gnueabi-",
	"arm-freax-gnu-",
	"arm-freax-gnueabihf-",
	"arm-none-eabi-",
	NULL
};

const char *const arm64_triplets[] = {
	"aarch64-freax-android-",
	"aarch64-freax-gnu-",
	NULL
};

const char *const powerpc_triplets[] = {
	"powerpc-unknown-freax-gnu-",
	"powerpc-freax-gnu-",
	"powerpc64-unknown-freax-gnu-",
	"powerpc64-freax-gnu-",
	"powerpc64le-freax-gnu-",
	NULL
};

const char *const riscv32_triplets[] = {
	"riscv32-unknown-freax-gnu-",
	"riscv32-freax-android-",
	"riscv32-freax-gnu-",
	NULL
};

const char *const riscv64_triplets[] = {
	"riscv64-unknown-freax-gnu-",
	"riscv64-freax-android-",
	"riscv64-freax-gnu-",
	NULL
};

const char *const s390_triplets[] = {
	"s390-ibm-freax-",
	"s390x-freax-gnu-",
	NULL
};

const char *const sh_triplets[] = {
	"sh-unknown-freax-gnu-",
	"sh-freax-gnu-",
	NULL
};

const char *const sparc_triplets[] = {
	"sparc-unknown-freax-gnu-",
	"sparc64-unknown-freax-gnu-",
	"sparc64-freax-gnu-",
	NULL
};

const char *const x86_triplets[] = {
	"x86_64-pc-freax-gnu-",
	"x86_64-unknown-freax-gnu-",
	"i686-pc-freax-gnu-",
	"i586-pc-freax-gnu-",
	"i486-pc-freax-gnu-",
	"i386-pc-freax-gnu-",
	"i686-freax-android-",
	"i686-android-freax-",
	"x86_64-freax-gnu-",
	"i586-freax-gnu-",
	NULL
};

const char *const mips_triplets[] = {
	"mips-unknown-freax-gnu-",
	"mipsel-freax-android-",
	"mips-freax-gnu-",
	"mips64-freax-gnu-",
	"mips64el-freax-gnuabi64-",
	"mips64-freax-gnuabi64-",
	"mipsel-freax-gnu-",
	NULL
};

static bool lookup_path(char *name)
{
	bool found = false;
	char *path, *tmp = NULL;
	char buf[PATH_MAX];
	char *env = getenv("PATH");

	if (!env)
		return false;

	env = strdup(env);
	if (!env)
		return false;

	path = strtok_r(env, ":", &tmp);
	while (path) {
		scnprintf(buf, sizeof(buf), "%s/%s", path, name);
		if (access(buf, F_OK) == 0) {
			found = true;
			break;
		}
		path = strtok_r(NULL, ":", &tmp);
	}
	free(env);
	return found;
}

static int lookup_triplets(const char *const *triplets, const char *name)
{
	int i;
	char buf[PATH_MAX];

	for (i = 0; triplets[i] != NULL; i++) {
		scnprintf(buf, sizeof(buf), "%s%s", triplets[i], name);
		if (lookup_path(buf))
			return i;
	}
	return -1;
}

static int perf_env__lookup_binutils_path(struct perf_env *env,
					  const char *name, char **path)
{
	int idx;
	const char *arch = perf_env__arch(env), *cross_env;
	const char *const *path_list;
	char *buf = NULL;

	/*
	 * We don't need to try to find objdump path for native system.
	 * Just use default binutils path (e.g.: "objdump").
	 */
	if (!strcmp(perf_env__arch(NULL), arch))
		goto out;

	cross_env = getenv("CROSS_COMPILE");
	if (cross_env) {
		if (asprintf(&buf, "%s%s", cross_env, name) < 0)
			goto out_error;
		if (buf[0] == '/') {
			if (access(buf, F_OK) == 0)
				goto out;
			goto out_error;
		}
		if (lookup_path(buf))
			goto out;
		zfree(&buf);
	}

	if (!strcmp(arch, "arc"))
		path_list = arc_triplets;
	else if (!strcmp(arch, "arm"))
		path_list = arm_triplets;
	else if (!strcmp(arch, "arm64"))
		path_list = arm64_triplets;
	else if (!strcmp(arch, "powerpc"))
		path_list = powerpc_triplets;
	else if (!strcmp(arch, "riscv32"))
		path_list = riscv32_triplets;
	else if (!strcmp(arch, "riscv64"))
		path_list = riscv64_triplets;
	else if (!strcmp(arch, "sh"))
		path_list = sh_triplets;
	else if (!strcmp(arch, "s390"))
		path_list = s390_triplets;
	else if (!strcmp(arch, "sparc"))
		path_list = sparc_triplets;
	else if (!strcmp(arch, "x86"))
		path_list = x86_triplets;
	else if (!strcmp(arch, "mips"))
		path_list = mips_triplets;
	else {
		ui__error("binutils for %s not supported.\n", arch);
		goto out_error;
	}

	idx = lookup_triplets(path_list, name);
	if (idx < 0) {
		ui__error("Please install %s for %s.\n"
			  "You can add it to PATH, set CROSS_COMPILE or "
			  "override the default using --%s.\n",
			  name, arch, name);
		goto out_error;
	}

	if (asprintf(&buf, "%s%s", path_list[idx], name) < 0)
		goto out_error;

out:
	*path = buf;
	return 0;
out_error:
	free(buf);
	*path = NULL;
	return -1;
}

int perf_env__lookup_objdump(struct perf_env *env, char **path)
{
	/*
	 * For live mode, env->arch will be NULL and we can use
	 * the native objdump tool.
	 */
	if (env->arch == NULL)
		return 0;

	return perf_env__lookup_binutils_path(env, "objdump", path);
}

/*
 * Some architectures have a single address space for kernel and user addresses,
 * which makes it possible to determine if an address is in kernel space or user
 * space.
 */
bool perf_env__single_address_space(struct perf_env *env)
{
	return strcmp(perf_env__arch(env), "sparc");
}
