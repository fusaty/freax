/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017-2018 Bartosz Golaszewski <brgl@bgdev.pl>
 * Copyright (C) 2020 Bartosz Golaszewski <bgolaszewski@baylibre.com>
 */

#ifndef _freax_IRQ_SIM_H
#define _freax_IRQ_SIM_H

#include <freax/device.h>
#include <freax/fwnode.h>
#include <freax/irqdomain.h>

/*
 * Provides a framework for allocating simulated interrupts which can be
 * requested like normal irqs and enqueued from process context.
 */

struct irq_domain *irq_domain_create_sim(struct fwnode_handle *fwnode,
					 unsigned int num_irqs);
struct irq_domain *devm_irq_domain_create_sim(struct device *dev,
					      struct fwnode_handle *fwnode,
					      unsigned int num_irqs);
void irq_domain_remove_sim(struct irq_domain *domain);

#endif /* _freax_IRQ_SIM_H */
