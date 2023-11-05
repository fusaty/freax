/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Lakshmi Ramasubramanian (nramas@freax.microsoft.com)
 *
 * Measure critical data structures maintained by SEfreax
 * using IMA subsystem.
 */

#ifndef _SEfreax_IMA_H_
#define _SEfreax_IMA_H_

#include "security.h"

#ifdef CONFIG_IMA
extern void sefreax_ima_measure_state(void);
extern void sefreax_ima_measure_state_locked(void);
#else
static inline void sefreax_ima_measure_state(void)
{
}
static inline void sefreax_ima_measure_state_locked(void)
{
}
#endif

#endif	/* _SEfreax_IMA_H_ */
