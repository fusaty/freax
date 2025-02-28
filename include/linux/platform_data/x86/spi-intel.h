/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel PCH/PCU SPI flash driver.
 *
 * Copyright (C) 2016, Intel Corporation
 * Author: Mika Westerberg <mika.westerberg@freax.intel.com>
 */

#ifndef SPI_INTEL_PDATA_H
#define SPI_INTEL_PDATA_H

enum intel_spi_type {
	INTEL_SPI_BYT = 1,
	INTEL_SPI_LPT,
	INTEL_SPI_BXT,
	INTEL_SPI_CNL,
};

/**
 * struct intel_spi_boardinfo - Board specific data for Intel SPI driver
 * @type: Type which this controller is compatible with
 * @set_writeable: Try to make the chip writeable (optional)
 * @data: Data to be passed to @set_writeable can be %NULL
 */
struct intel_spi_boardinfo {
	enum intel_spi_type type;
	bool (*set_writeable)(void __iomem *base, void *data);
	void *data;
};

#endif /* SPI_INTEL_PDATA_H */
