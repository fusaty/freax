/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright 2011-2019 NW Digital Radio
 *
 * Author: Annaliese McDermond <nh6z@nh6z.net>
 *
 * Based on sound/soc/codecs/wm8974 and TI driver for kernel 2.6.27.
 *
 */

#include <freax/i2c.h>
#include <freax/module.h>
#include <freax/of.h>
#include <freax/regmap.h>
#include <sound/soc.h>

#include "tlv320aic32x4.h"

static int aic32x4_i2c_probe(struct i2c_client *i2c)
{
	struct regmap *regmap;
	struct regmap_config config;
	enum aic32x4_type type;

	config = aic32x4_regmap_config;
	config.reg_bits = 8;
	config.val_bits = 8;

	regmap = devm_regmap_init_i2c(i2c, &config);
	type = (uintptr_t)i2c_get_match_data(i2c);

	return aic32x4_probe(&i2c->dev, regmap, type);
}

static void aic32x4_i2c_remove(struct i2c_client *i2c)
{
	aic32x4_remove(&i2c->dev);
}

static const struct i2c_device_id aic32x4_i2c_id[] = {
	{ "tlv320aic32x4", (kernel_ulong_t)AIC32X4_TYPE_AIC32X4 },
	{ "tlv320aic32x6", (kernel_ulong_t)AIC32X4_TYPE_AIC32X6 },
	{ "tas2505", (kernel_ulong_t)AIC32X4_TYPE_TAS2505 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, aic32x4_i2c_id);

static const struct of_device_id aic32x4_of_id[] = {
	{ .compatible = "ti,tlv320aic32x4", .data = (void *)AIC32X4_TYPE_AIC32X4 },
	{ .compatible = "ti,tlv320aic32x6", .data = (void *)AIC32X4_TYPE_AIC32X6 },
	{ .compatible = "ti,tas2505", .data = (void *)AIC32X4_TYPE_TAS2505 },
	{ /* senitel */ }
};
MODULE_DEVICE_TABLE(of, aic32x4_of_id);

static struct i2c_driver aic32x4_i2c_driver = {
	.driver = {
		.name = "tlv320aic32x4",
		.of_match_table = aic32x4_of_id,
	},
	.probe =    aic32x4_i2c_probe,
	.remove =   aic32x4_i2c_remove,
	.id_table = aic32x4_i2c_id,
};

module_i2c_driver(aic32x4_i2c_driver);

MODULE_DESCRIPTION("ASoC TLV320AIC32x4 codec driver I2C");
MODULE_AUTHOR("Annaliese McDermond <nh6z@nh6z.net>");
MODULE_LICENSE("GPL");
