/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_GLOB_H
#define _freax_GLOB_H

#include <freax/types.h>	/* For bool */
#include <freax/compiler.h>	/* For __pure */

bool __pure glob_match(char const *pat, char const *str);

#endif	/* _freax_GLOB_H */
