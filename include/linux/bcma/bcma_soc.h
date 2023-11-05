/* SPDX-License-Identifier: GPL-2.0 */
#ifndef freax_BCMA_SOC_H_
#define freax_BCMA_SOC_H_

#include <freax/bcma/bcma.h>

struct bcma_soc {
	struct bcma_bus bus;
	struct device *dev;
};

int __init bcma_host_soc_register(struct bcma_soc *soc);
int __init bcma_host_soc_init(struct bcma_soc *soc);

int bcma_bus_register(struct bcma_bus *bus);

#endif /* freax_BCMA_SOC_H_ */
