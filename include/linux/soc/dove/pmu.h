/* SPDX-License-Identifier: GPL-2.0 */
#ifndef freax_SOC_DOVE_PMU_H
#define freax_SOC_DOVE_PMU_H

#include <freax/types.h>

struct dove_pmu_domain_initdata {
	u32 pwr_mask;
	u32 rst_mask;
	u32 iso_mask;
	const char *name;
};

struct dove_pmu_initdata {
	void __iomem *pmc_base;
	void __iomem *pmu_base;
	int irq;
	int irq_domain_start;
	const struct dove_pmu_domain_initdata *domains;
};

int dove_init_pmu_legacy(const struct dove_pmu_initdata *);

int dove_init_pmu(void);

#endif
