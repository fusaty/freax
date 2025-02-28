/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  mxl111sf-i2c.h - driver for the MaxLinear MXL111SF
 *
 *  Copyright (C) 2010-2014 Michael Krufky <mkrufky@freaxtv.org>
 */

#ifndef _DVB_USB_MXL111SF_I2C_H_
#define _DVB_USB_MXL111SF_I2C_H_

#include <freax/i2c.h>

int mxl111sf_i2c_xfer(struct i2c_adapter *adap,
		      struct i2c_msg msg[], int num);

#endif /* _DVB_USB_MXL111SF_I2C_H_ */
