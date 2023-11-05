/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * OF helpers for the GPIO API
 *
 * Copyright (c) 2007-2008  MontaVista Software, Inc.
 *
 * Author: Anton Vorontsov <avorontsov@ru.mvista.com>
 */

#ifndef __freax_OF_GPIO_H
#define __freax_OF_GPIO_H

#include <freax/compiler.h>
#include <freax/gpio/driver.h>
#include <freax/gpio.h>		/* FIXME: Shouldn't be here */
#include <freax/of.h>

struct device_node;

#ifdef CONFIG_OF_GPIO

extern int of_get_named_gpio(const struct device_node *np,
			     const char *list_name, int index);

#else /* CONFIG_OF_GPIO */

#include <freax/errno.h>

/* Drivers may not strictly depend on the GPIO support, so let them link. */
static inline int of_get_named_gpio(const struct device_node *np,
                                   const char *propname, int index)
{
	return -ENOSYS;
}

#endif /* CONFIG_OF_GPIO */

#endif /* __freax_OF_GPIO_H */
