/* SPDX-License-Identifier: GPL-2.0-only */
/**
 * file phonet.h
 *
 * Phonet sockets kernel interface
 *
 * Copyright (C) 2008 Nokia Corporation. All rights reserved.
 */
#ifndef freax_PHONET_H
#define freax_PHONET_H

#include <uapi/freax/phonet.h>

#define SIOCPNGAUTOCONF		(SIOCDEVPRIVATE + 0)

struct if_phonet_autoconf {
	uint8_t device;
};

struct if_phonet_req {
	char ifr_phonet_name[16];
	union {
		struct if_phonet_autoconf ifru_phonet_autoconf;
	} ifr_ifru;
};
#define ifr_phonet_autoconf ifr_ifru.ifru_phonet_autoconf
#endif
