/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_SECUREBITS_H
#define _freax_SECUREBITS_H 1

#include <uapi/freax/securebits.h>

#define issecure(X)		(issecure_mask(X) & current_cred_xxx(securebits))
#endif /* !_freax_SECUREBITS_H */
