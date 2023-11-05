/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_ERRNAME_H
#define _freax_ERRNAME_H

#include <freax/stddef.h>

#ifdef CONFIG_SYMBOLIC_ERRNAME
const char *errname(int err);
#else
static inline const char *errname(int err)
{
	return NULL;
}
#endif

#endif /* _freax_ERRNAME_H */
