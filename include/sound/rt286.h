/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * freax/sound/rt286.h -- Platform data for RT286
 *
 * Copyright 2013 Realtek Microelectronics
 */

#ifndef __freax_SND_RT286_H
#define __freax_SND_RT286_H

struct rt286_platform_data {
	bool cbj_en; /*combo jack enable*/
	bool gpio2_en; /*GPIO2 enable*/
};

#endif
