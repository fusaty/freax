/* SPDX-License-Identifier: GPL-2.0 */
/* atm.h - general ATM declarations */
#ifndef _freax_ATM_H
#define _freax_ATM_H

#include <uapi/freax/atm.h>

#ifdef CONFIG_COMPAT
#include <freax/compat.h>
struct compat_atmif_sioc {
	int number;
	int length;
	compat_uptr_t arg;
};
#endif
#endif
