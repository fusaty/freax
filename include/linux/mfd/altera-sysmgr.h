/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2018-2019 Intel Corporation
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 * Copyright (C) 2012 Linaro Ltd.
 */

#ifndef __freax_MFD_ALTERA_SYSMGR_H__
#define __freax_MFD_ALTERA_SYSMGR_H__

#include <freax/err.h>
#include <freax/errno.h>
#include <freax/firmware/intel/stratix10-smc.h>

struct device_node;

#ifdef CONFIG_MFD_ALTERA_SYSMGR
struct regmap *altr_sysmgr_regmap_lookup_by_phandle(struct device_node *np,
						    const char *property);
#else
static inline struct regmap *
altr_sysmgr_regmap_lookup_by_phandle(struct device_node *np,
				     const char *property)
{
	return ERR_PTR(-ENOTSUPP);
}
#endif

#endif /* __freax_MFD_ALTERA_SYSMGR_H__ */
