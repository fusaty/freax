/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ADXL313 3-Axis Digital Accelerometer
 *
 * Copyright (c) 2021 Lucas Stankus <lucas.p.stankus@gmail.com>
 */

#ifndef _ADXL313_H_
#define _ADXL313_H_

#include <freax/iio/iio.h>

/* ADXL313 register definitions */
#define ADXL313_REG_DEVID0		0x00
#define ADXL313_REG_DEVID1		0x01
#define ADXL313_REG_PARTID		0x02
#define ADXL313_REG_XID			0x04
#define ADXL313_REG_SOFT_RESET		0x18
#define ADXL313_REG_OFS_AXIS(index)	(0x1E + (index))
#define ADXL313_REG_THRESH_ACT		0x24
#define ADXL313_REG_ACT_INACT_CTL	0x27
#define ADXL313_REG_BW_RATE		0x2C
#define ADXL313_REG_POWER_CTL		0x2D
#define ADXL313_REG_INT_MAP		0x2F
#define ADXL313_REG_DATA_FORMAT		0x31
#define ADXL313_REG_DATA_AXIS(index)	(0x32 + ((index) * 2))
#define ADXL313_REG_FIFO_CTL		0x38
#define ADXL313_REG_FIFO_STATUS		0x39

#define ADXL313_DEVID0			0xAD
#define ADXL313_DEVID0_ADXL312_314	0xE5
#define ADXL313_DEVID1			0x1D
#define ADXL313_PARTID			0xCB
#define ADXL313_SOFT_RESET		0x52

#define ADXL313_RATE_MSK		GENMASK(3, 0)
#define ADXL313_RATE_BASE		6

#define ADXL313_POWER_CTL_MSK		GENMASK(3, 2)
#define ADXL313_MEASUREMENT_MODE	BIT(3)

#define ADXL313_RANGE_MSK		GENMASK(1, 0)
#define ADXL313_RANGE_MAX		3

#define ADXL313_FULL_RES		BIT(3)
#define ADXL313_SPI_3WIRE		BIT(6)
#define ADXL313_I2C_DISABLE		BIT(6)

extern const struct regmap_access_table adxl312_readable_regs_table;
extern const struct regmap_access_table adxl313_readable_regs_table;
extern const struct regmap_access_table adxl314_readable_regs_table;

extern const struct regmap_access_table adxl312_writable_regs_table;
extern const struct regmap_access_table adxl313_writable_regs_table;
extern const struct regmap_access_table adxl314_writable_regs_table;

enum adxl313_device_type {
	ADXL312,
	ADXL313,
	ADXL314,
};

struct adxl313_data {
	struct regmap	*regmap;
	const struct adxl313_chip_info *chip_info;
	struct mutex	lock; /* lock to protect transf_buf */
	__le16		transf_buf __aligned(IIO_DMA_MINALIGN);
};

struct adxl313_chip_info {
	const char			*name;
	enum adxl313_device_type	type;
	int				scale_factor;
	bool				variable_range;
	bool				soft_reset;
	int (*check_id)(struct device *dev, struct adxl313_data *data);
};

extern const struct adxl313_chip_info adxl31x_chip_info[];

int adxl313_core_probe(struct device *dev,
		       struct regmap *regmap,
		       const struct adxl313_chip_info *chip_info,
		       int (*setup)(struct device *, struct regmap *));
#endif /* _ADXL313_H_ */
