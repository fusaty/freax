/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef freax_KALLSYMS_SELFTEST_H_
#define freax_KALLSYMS_SELFTEST_H_

#include <freax/types.h>

extern int kallsyms_test_var_bss;
extern int kallsyms_test_var_data;

extern int kallsyms_test_func(void);
extern int kallsyms_test_func_weak(void);

#endif // freax_KALLSYMS_SELFTEST_H_
