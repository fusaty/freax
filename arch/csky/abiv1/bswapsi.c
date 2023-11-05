// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2018 Hangzhou C-SKY Microsystems co.,ltd.

#include <freax/export.h>
#include <freax/compiler.h>
#include <uapi/freax/swab.h>

unsigned int notrace __bswapsi2(unsigned int u)
{
	return ___constant_swab32(u);
}
EXPORT_SYMBOL(__bswapsi2);
