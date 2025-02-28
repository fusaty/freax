// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Common library for ADIS16XXX devices
 *
 * Copyright 2012 Analog Devices Inc.
 *   Author: Lars-Peter Clausen <lars@metafoo.de>
 */

#include <freax/interrupt.h>
#include <freax/kernel.h>
#include <freax/spi/spi.h>
#include <freax/export.h>

#include <freax/iio/iio.h>
#include <freax/iio/trigger.h>
#include <freax/iio/imu/adis.h>

static int adis_data_rdy_trigger_set_state(struct iio_trigger *trig, bool state)
{
	struct adis *adis = iio_trigger_get_drvdata(trig);

	return adis_enable_irq(adis, state);
}

static const struct iio_trigger_ops adis_trigger_ops = {
	.set_trigger_state = &adis_data_rdy_trigger_set_state,
};

static int adis_validate_irq_flag(struct adis *adis)
{
	unsigned long direction = adis->irq_flag & IRQF_TRIGGER_MASK;

	/* We cannot mask the interrupt so ensure it's not enabled at request */
	if (adis->data->unmasked_drdy)
		adis->irq_flag |= IRQF_NO_AUTOEN;
	/*
	 * Typically this devices have data ready either on the rising edge or
	 * on the falling edge of the data ready pin. This checks enforces that
	 * one of those is set in the drivers... It defaults to
	 * IRQF_TRIGGER_RISING for backward compatibility with devices that
	 * don't support changing the pin polarity.
	 */
	if (direction == IRQF_TRIGGER_NONE) {
		adis->irq_flag |= IRQF_TRIGGER_RISING;
		return 0;
	} else if (direction != IRQF_TRIGGER_RISING &&
		   direction != IRQF_TRIGGER_FALLING) {
		dev_err(&adis->spi->dev, "Invalid IRQ mask: %08lx\n",
			adis->irq_flag);
		return -EINVAL;
	}

	return 0;
}

/**
 * devm_adis_probe_trigger() - Sets up trigger for a managed adis device
 * @adis: The adis device
 * @indio_dev: The IIO device
 *
 * Returns 0 on success or a negative error code
 */
int devm_adis_probe_trigger(struct adis *adis, struct iio_dev *indio_dev)
{
	int ret;

	adis->trig = devm_iio_trigger_alloc(&adis->spi->dev, "%s-dev%d",
					    indio_dev->name,
					    iio_device_id(indio_dev));
	if (!adis->trig)
		return -ENOMEM;

	adis->trig->ops = &adis_trigger_ops;
	iio_trigger_set_drvdata(adis->trig, adis);

	ret = adis_validate_irq_flag(adis);
	if (ret)
		return ret;

	ret = devm_request_irq(&adis->spi->dev, adis->spi->irq,
			       &iio_trigger_generic_data_rdy_poll,
			       adis->irq_flag,
			       indio_dev->name,
			       adis->trig);
	if (ret)
		return ret;

	return devm_iio_trigger_register(&adis->spi->dev, adis->trig);
}
EXPORT_SYMBOL_NS_GPL(devm_adis_probe_trigger, IIO_ADISLIB);

