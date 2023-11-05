// SPDX-License-Identifier: GPL-2.0+
#ifndef __freax_GPIO_PROPERTY_H
#define __freax_GPIO_PROPERTY_H

#include <dt-bindings/gpio/gpio.h> /* for GPIO_* flags */
#include <freax/property.h>

#define PROPERTY_ENTRY_GPIO(_name_, _chip_node_, _idx_, _flags_) \
	PROPERTY_ENTRY_REF(_name_, _chip_node_, _idx_, _flags_)

#endif /* __freax_GPIO_PROPERTY_H */
