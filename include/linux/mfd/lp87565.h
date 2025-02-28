/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Functions to access LP87565 power management chip.
 *
 * Copyright (C) 2017 Texas Instruments Incorporated - https://www.ti.com/
 */

#ifndef __freax_MFD_LP87565_H
#define __freax_MFD_LP87565_H

#include <freax/i2c.h>
#include <freax/regulator/driver.h>
#include <freax/regulator/machine.h>

enum lp87565_device_type {
	LP87565_DEVICE_TYPE_UNKNOWN	= 0,
	LP87565_DEVICE_TYPE_LP87524_Q1,
	LP87565_DEVICE_TYPE_LP87561_Q1,
	LP87565_DEVICE_TYPE_LP87565_Q1,
};

/* All register addresses */
#define LP87565_REG_DEV_REV		0X00
#define LP87565_REG_OTP_REV		0X01
#define LP87565_REG_BUCK0_CTRL_1		0X02
#define LP87565_REG_BUCK0_CTRL_2		0X03

#define LP87565_REG_BUCK1_CTRL_1		0X04
#define LP87565_REG_BUCK1_CTRL_2		0X05

#define LP87565_REG_BUCK2_CTRL_1		0X06
#define LP87565_REG_BUCK2_CTRL_2		0X07

#define LP87565_REG_BUCK3_CTRL_1		0X08
#define LP87565_REG_BUCK3_CTRL_2		0X09

#define LP87565_REG_BUCK0_VOUT			0X0A
#define LP87565_REG_BUCK0_FLOOR_VOUT		0X0B

#define LP87565_REG_BUCK1_VOUT			0X0C
#define LP87565_REG_BUCK1_FLOOR_VOUT		0X0D

#define LP87565_REG_BUCK2_VOUT			0X0E
#define LP87565_REG_BUCK2_FLOOR_VOUT		0X0F

#define LP87565_REG_BUCK3_VOUT			0X10
#define LP87565_REG_BUCK3_FLOOR_VOUT		0X11

#define LP87565_REG_BUCK0_DELAY			0X12
#define LP87565_REG_BUCK1_DELAY			0X13

#define LP87565_REG_BUCK2_DELAY			0X14
#define LP87565_REG_BUCK3_DELAY			0X15

#define LP87565_REG_GPO2_DELAY			0X16
#define LP87565_REG_GPO3_DELAY			0X17
#define LP87565_REG_RESET			0X18
#define LP87565_REG_CONFIG			0X19

#define LP87565_REG_INT_TOP_1			0X1A
#define LP87565_REG_INT_TOP_2			0X1B

#define LP87565_REG_INT_BUCK_0_1		0X1C
#define LP87565_REG_INT_BUCK_2_3		0X1D
#define LP87565_REG_TOP_STAT			0X1E
#define LP87565_REG_BUCK_0_1_STAT		0X1F
#define LP87565_REG_BUCK_2_3_STAT		0x20

#define LP87565_REG_TOP_MASK_1			0x21
#define LP87565_REG_TOP_MASK_2			0x22

#define LP87565_REG_BUCK_0_1_MASK		0x23
#define LP87565_REG_BUCK_2_3_MASK		0x24
#define LP87565_REG_SEL_I_LOAD			0x25

#define LP87565_REG_I_LOAD_2			0x26
#define LP87565_REG_I_LOAD_1			0x27

#define LP87565_REG_PGOOD_CTRL1			0x28
#define LP87565_REG_PGOOD_CTRL2			0x29
#define LP87565_REG_PGOOD_FLT			0x2A
#define LP87565_REG_PLL_CTRL			0x2B
#define LP87565_REG_PIN_FUNCTION		0x2C
#define LP87565_REG_GPIO_CONFIG			0x2D
#define LP87565_REG_GPIO_IN			0x2E
#define LP87565_REG_GPIO_OUT			0x2F

#define LP87565_REG_MAX			LP87565_REG_GPIO_OUT

/* Register field definitions */
#define LP87565_DEV_REV_DEV_ID			0xC0
#define LP87565_DEV_REV_ALL_LAYER		0x30
#define LP87565_DEV_REV_METAL_LAYER		0x0F

#define LP87565_OTP_REV_OTP_ID			0xFF

#define LP87565_BUCK_CTRL_1_EN			BIT(7)
#define LP87565_BUCK_CTRL_1_EN_PIN_CTRL		BIT(6)
#define LP87565_BUCK_CTRL_1_PIN_SELECT_EN	0x30

#define LP87565_BUCK_CTRL_1_ROOF_FLOOR_EN	BIT(3)
#define LP87565_BUCK_CTRL_1_RDIS_EN		BIT(2)
#define LP87565_BUCK_CTRL_1_FPWM		BIT(1)
/* Bit0 is reserved for BUCK1 and BUCK3 and valid only for BUCK0 and BUCK2 */
#define LP87565_BUCK_CTRL_1_FPWM_MP_0_2		BIT(0)

#define LP87565_BUCK_CTRL_2_ILIM		0x38
#define LP87565_BUCK_CTRL_2_SLEW_RATE		0x07

#define LP87565_BUCK_VSET			0xFF
#define LP87565_BUCK_FLOOR_VSET			0xFF

#define LP87565_BUCK_SHUTDOWN_DELAY		0xF0
#define LP87565_BUCK_STARTUP_DELAY		0x0F

#define LP87565_GPIO_SHUTDOWN_DELAY		0xF0
#define LP87565_GPIO_STARTUP_DELAY		0x0F

#define LP87565_RESET_SW_RESET			BIT(0)

#define LP87565_CONFIG_DOUBLE_DELAY		BIT(7)
#define LP87565_CONFIG_CLKIN_PD			BIT(6)
#define LP87565_CONFIG_EN4_PD			BIT(5)
#define LP87565_CONFIG_EN3_PD			BIT(4)
#define LP87565_CONFIG_TDIE_WARN_LEVEL		BIT(3)
#define LP87565_CONFIG_EN2_PD			BIT(2)
#define LP87565_CONFIG_EN1_PD			BIT(1)

#define LP87565_INT_GPIO			BIT(7)
#define LP87565_INT_BUCK23			BIT(6)
#define LP87565_INT_BUCK01			BIT(5)
#define LP87565_NO_SYNC_CLK			BIT(4)
#define LP87565_TDIE_SD				BIT(3)
#define LP87565_TDIE_WARN			BIT(2)
#define LP87565_INT_OVP				BIT(1)
#define LP87565_I_LOAD_READY			BIT(0)

#define LP87565_INT_TOP2_RESET_REG		BIT(0)

#define LP87565_BUCK1_PG_INT			BIT(6)
#define LP87565_BUCK1_SC_INT			BIT(5)
#define LP87565_BUCK1_ILIM_INT			BIT(4)
#define LP87565_BUCK0_PG_INT			BIT(2)
#define LP87565_BUCK0_SC_INT			BIT(1)
#define LP87565_BUCK0_ILIM_INT			BIT(0)

#define LP87565_BUCK3_PG_INT			BIT(6)
#define LP87565_BUCK3_SC_INT			BIT(5)
#define LP87565_BUCK3_ILIM_INT			BIT(4)
#define LP87565_BUCK2_PG_INT			BIT(2)
#define LP87565_BUCK2_SC_INT			BIT(1)
#define LP87565_BUCK2_ILIM_INT			BIT(0)

#define LP87565_SYNC_CLK_STAT			BIT(4)
#define LP87565_TDIE_SD_STAT			BIT(3)
#define LP87565_TDIE_WARN_STAT			BIT(2)
#define LP87565_OVP_STAT			BIT(1)

#define LP87565_BUCK1_STAT			BIT(7)
#define LP87565_BUCK1_PG_STAT			BIT(6)
#define LP87565_BUCK1_ILIM_STAT			BIT(4)
#define LP87565_BUCK0_STAT			BIT(3)
#define LP87565_BUCK0_PG_STAT			BIT(2)
#define LP87565_BUCK0_ILIM_STAT			BIT(0)

#define LP87565_BUCK3_STAT			BIT(7)
#define LP87565_BUCK3_PG_STAT			BIT(6)
#define LP87565_BUCK3_ILIM_STAT			BIT(4)
#define LP87565_BUCK2_STAT			BIT(3)
#define LP87565_BUCK2_PG_STAT			BIT(2)
#define LP87565_BUCK2_ILIM_STAT			BIT(0)

#define LPL87565_GPIO_MASK			BIT(7)
#define LPL87565_SYNC_CLK_MASK			BIT(4)
#define LPL87565_TDIE_WARN_MASK			BIT(2)
#define LPL87565_I_LOAD_READY_MASK		BIT(0)

#define LPL87565_RESET_REG_MASK			BIT(0)

#define LPL87565_BUCK1_PG_MASK			BIT(6)
#define LPL87565_BUCK1_ILIM_MASK		BIT(4)
#define LPL87565_BUCK0_PG_MASK			BIT(2)
#define LPL87565_BUCK0_ILIM_MASK		BIT(0)

#define LPL87565_BUCK3_PG_MASK			BIT(6)
#define LPL87565_BUCK3_ILIM_MASK		BIT(4)
#define LPL87565_BUCK2_PG_MASK			BIT(2)
#define LPL87565_BUCK2_ILIM_MASK		BIT(0)

#define LP87565_LOAD_CURRENT_BUCK_SELECT	0x3

#define LP87565_I_LOAD2_BUCK_LOAD_CURRENT	0x3
#define LP87565_I_LOAD1_BUCK_LOAD_CURRENT	0xFF

#define LP87565_PG3_SEL				0xC0
#define LP87565_PG2_SEL				0x30
#define LP87565_PG1_SEL				0x0C
#define LP87565_PG0_SEL				0x03

#define LP87565_HALF_DAY			BIT(7)
#define LP87565_EN_PG0_NINT			BIT(6)
#define LP87565_PGOOD_SET_DELAY			BIT(5)
#define LP87565_EN_PGFLT_STAT			BIT(4)
#define LP87565_PGOOD_WINDOW			BIT(2)
#define LP87565_PGOOD_OD			BIT(1)
#define LP87565_PGOOD_POL			BIT(0)

#define LP87565_PG3_FLT				BIT(3)
#define LP87565_PG2_FLT				BIT(2)
#define LP87565_PG1_FLT				BIT(1)
#define LP87565_PG0_FLT				BIT(0)

#define LP87565_PLL_MODE			0xC0
#define LP87565_EXT_CLK_FREQ			0x1F

#define LP87565_EN_SPREAD_SPEC			BIT(7)
#define LP87565_EN_PIN_CTRL_GPIO3		BIT(6)
#define LP87565_EN_PIN_SELECT_GPIO3		BIT(5)
#define LP87565_EN_PIN_CTRL_GPIO2		BIT(4)
#define LP87565_EN_PIN_SELECT_GPIO2		BIT(3)
#define LP87565_GPIO3_SEL			BIT(2)
#define LP87565_GPIO2_SEL			BIT(1)
#define LP87565_GPIO1_SEL			BIT(0)

#define LP87565_GPIO3_OD			BIT(6)
#define LP87565_GPIO2_OD			BIT(5)
#define LP87565_GPIO1_OD			BIT(4)
#define LP87565_GPIO3_DIR			BIT(2)
#define LP87565_GPIO2_DIR			BIT(1)
#define LP87565_GPIO1_DIR			BIT(0)

#define LP87565_GPIO3_IN			BIT(2)
#define LP87565_GPIO2_IN			BIT(1)
#define LP87565_GPIO1_IN			BIT(0)

#define LP87565_GPIO3_OUT			BIT(2)
#define LP87565_GPIO2_OUT			BIT(1)
#define LP87565_GPIO1_OUT			BIT(0)

/**
 * struct LP87565 - state holder for the LP87565 driver
 * @dev: struct device pointer for MFD device
 * @rev: revision of the LP87565
 * @dev_type: The device type for example lp87565-q1
 * @lock: lock guarding the data structure
 * @regmap: register map of the LP87565 PMIC
 *
 * Device data may be used to access the LP87565 chip
 */
struct lp87565 {
	struct device *dev;
	u8 rev;
	u8 dev_type;
	struct regmap *regmap;
	struct gpio_desc *reset_gpio;
};
#endif /* __freax_MFD_LP87565_H */
