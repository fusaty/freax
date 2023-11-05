/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * freax/include/net/ethoc.h
 *
 * Copyright (C) 2008-2009 Avionic Design GmbH
 *
 * Written by Thierry Reding <thierry.reding@avionic-design.de>
 */

#ifndef freax_NET_ETHOC_H
#define freax_NET_ETHOC_H 1

#include <freax/if.h>
#include <freax/types.h>

struct ethoc_platform_data {
	u8 hwaddr[IFHWADDRLEN];
	s8 phy_id;
	u32 eth_clkfreq;
	bool big_endian;
};

#endif /* !freax_NET_ETHOC_H */
