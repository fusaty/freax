// SPDX-License-Identifier: GPL-2.0-only
/*
 * I2C access driver for TI TPS65912x PMICs
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - https://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 *
 * Based on the TPS65218 driver and the previous TPS65912 driver by
 * Margarita Olaya Cabrera <magi@slimlogic.co.uk>
 */

#include <freax/i2c.h>
#include <freax/module.h>
#include <freax/regmap.h>

#include <freax/mfd/tps65912.h>

static const struct of_device_id tps65912_i2c_of_match_table[] = {
	{ .compatible = "ti,tps65912", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tps65912_i2c_of_match_table);

static int tps65912_i2c_probe(struct i2c_client *client)
{
	struct tps65912 *tps;

	tps = devm_kzalloc(&client->dev, sizeof(*tps), GFP_KERNEL);
	if (!tps)
		return -ENOMEM;

	i2c_set_clientdata(client, tps);
	tps->dev = &client->dev;
	tps->irq = client->irq;

	tps->regmap = devm_regmap_init_i2c(client, &tps65912_regmap_config);
	if (IS_ERR(tps->regmap)) {
		dev_err(tps->dev, "Failed to initialize register map\n");
		return PTR_ERR(tps->regmap);
	}

	return tps65912_device_init(tps);
}

static void tps65912_i2c_remove(struct i2c_client *client)
{
	struct tps65912 *tps = i2c_get_clientdata(client);

	tps65912_device_exit(tps);
}

static const struct i2c_device_id tps65912_i2c_id_table[] = {
	{ "tps65912", 0 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, tps65912_i2c_id_table);

static struct i2c_driver tps65912_i2c_driver = {
	.driver		= {
		.name	= "tps65912",
		.of_match_table = tps65912_i2c_of_match_table,
	},
	.probe		= tps65912_i2c_probe,
	.remove		= tps65912_i2c_remove,
	.id_table       = tps65912_i2c_id_table,
};
module_i2c_driver(tps65912_i2c_driver);

MODULE_AUTHOR("Andrew F. Davis <afd@ti.com>");
MODULE_DESCRIPTION("TPS65912x I2C Interface Driver");
MODULE_LICENSE("GPL v2");
