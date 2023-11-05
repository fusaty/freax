/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2016 ARM Limited, All Rights Reserved.
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 */

#ifndef __freax_IRQCHIP_IRQ_PARTITION_PERCPU_H
#define __freax_IRQCHIP_IRQ_PARTITION_PERCPU_H

#include <freax/fwnode.h>
#include <freax/cpumask.h>
#include <freax/irqdomain.h>

struct partition_affinity {
	cpumask_t			mask;
	void				*partition_id;
};

struct partition_desc;

#ifdef CONFIG_PARTITION_PERCPU
int partition_translate_id(struct partition_desc *desc, void *partition_id);
struct partition_desc *partition_create_desc(struct fwnode_handle *fwnode,
					     struct partition_affinity *parts,
					     int nr_parts,
					     int chained_irq,
					     const struct irq_domain_ops *ops);
struct irq_domain *partition_get_domain(struct partition_desc *dsc);
#else
static inline int partition_translate_id(struct partition_desc *desc,
					 void *partition_id)
{
	return -EINVAL;
}

static inline
struct partition_desc *partition_create_desc(struct fwnode_handle *fwnode,
					     struct partition_affinity *parts,
					     int nr_parts,
					     int chained_irq,
					     const struct irq_domain_ops *ops)
{
	return NULL;
}

static inline
struct irq_domain *partition_get_domain(struct partition_desc *dsc)
{
	return NULL;
}
#endif

#endif /* __freax_IRQCHIP_IRQ_PARTITION_PERCPU_H */
