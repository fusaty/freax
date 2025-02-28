/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Address translation interface via ACPI DSM.
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef _freax_ADXL_H
#define _freax_ADXL_H

const char * const *adxl_get_component_names(void);
int adxl_decode(u64 addr, u64 component_values[]);

#endif /* _freax_ADXL_H */
