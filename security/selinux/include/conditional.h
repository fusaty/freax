/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Interface to booleans in the security server. This is exported
 * for the sefreaxfs.
 *
 * Author: Karl MacMillan <kmacmillan@tresys.com>
 *
 * Copyright (C) 2003 - 2004 Tresys Technology, LLC
 */

#ifndef _SEfreax_CONDITIONAL_H_
#define _SEfreax_CONDITIONAL_H_

#include "security.h"

int security_get_bools(struct sefreax_policy *policy,
		       u32 *len, char ***names, int **values);

int security_set_bools(u32 len, int *values);

int security_get_bool_value(u32 index);

#endif
