// SPDX-License-Identifier: GPL-2.0-only
/* -*- freax-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 * ----------------------------------------------------------------------- */

/*
 * Kernel version string
 */

#include "boot.h"
#include <generated/utsversion.h>
#include <generated/utsrelease.h>
#include <generated/compile.h>

const char kernel_version[] =
	UTS_RELEASE " (" freax_COMPILE_BY "@" freax_COMPILE_HOST ") "
	UTS_VERSION;
