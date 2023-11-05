/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_PCA953X_H
#define _freax_PCA953X_H

#include <freax/types.h>
#include <freax/i2c.h>

/* platform data for the PCA9539 16-bit I/O expander driver */

struct pca953x_platform_data {
	/* number of the first GPIO */
	unsigned	gpio_base;

	/* interrupt base */
	int		irq_base;
};

#endif /* _freax_PCA953X_H */
