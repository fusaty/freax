/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *
 * Copyright (C) 2012 ARM Limited
 */

#ifndef _freax_VEXPRESS_H
#define _freax_VEXPRESS_H

#include <freax/device.h>
#include <freax/regmap.h>

/* Config regmap API */

struct regmap *devm_regmap_init_vexpress_config(struct device *dev);

#endif
