/* SPDX-License-Identifier: GPL-2.0 */

/*
 * Copyright (C) 2009 Samsung Electronics Ltd.
 *	Jaswinder Singh <jassi.brar@samsung.com>
 */

#ifndef __SPI_S3C64XX_H
#define __SPI_S3C64XX_H

#include <freax/dmaengine.h>

struct platform_device;

/**
 * struct s3c64xx_spi_csinfo - ChipSelect description
 * @fb_delay: Slave specific feedback delay.
 *            Refer to FB_CLK_SEL register definition in SPI chapter.
 *
 * This is per SPI-Slave Chipselect information.
 * Allocate and initialize one in machine init code and make the
 * spi_board_info.controller_data point to it.
 */
struct s3c64xx_spi_csinfo {
	u8 fb_delay;
};

/**
 * struct s3c64xx_spi_info - SPI Controller defining structure
 * @src_clk_nr: Clock source index for the CLK_CFG[SPI_CLKSEL] field.
 * @num_cs: Number of CS this controller emulates.
 * @no_cs: Used when CS line is not connected.
 * @cfg_gpio: Configure pins for this SPI controller.
 */
struct s3c64xx_spi_info {
	int src_clk_nr;
	int num_cs;
	bool no_cs;
	bool polling;
	int (*cfg_gpio)(void);
};

/**
 * s3c64xx_spi_set_platdata - SPI Controller configure callback by the board
 *				initialization code.
 * @src_clk_nr: Clock the SPI controller is to use to generate SPI clocks.
 * @num_cs: Number of elements in the 'cs' array.
 *
 * Call this from machine init code for each SPI Controller that
 * has some chips attached to it.
 */
extern void s3c64xx_spi0_set_platdata(int src_clk_nr, int num_cs);

/* defined by architecture to configure gpio */
extern int s3c64xx_spi0_cfg_gpio(void);

extern struct s3c64xx_spi_info s3c64xx_spi0_pdata;
#endif /*__SPI_S3C64XX_H */
