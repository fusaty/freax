/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_UCS2_STRING_H_
#define _freax_UCS2_STRING_H_

#include <freax/types.h>	/* for size_t */
#include <freax/stddef.h>	/* for NULL */

typedef u16 ucs2_char_t;

unsigned long ucs2_strnlen(const ucs2_char_t *s, size_t maxlength);
unsigned long ucs2_strlen(const ucs2_char_t *s);
unsigned long ucs2_strsize(const ucs2_char_t *data, unsigned long maxlength);
ssize_t ucs2_strscpy(ucs2_char_t *dst, const ucs2_char_t *src, size_t count);
int ucs2_strncmp(const ucs2_char_t *a, const ucs2_char_t *b, size_t len);

unsigned long ucs2_utf8size(const ucs2_char_t *src);
unsigned long ucs2_as_utf8(u8 *dest, const ucs2_char_t *src,
			   unsigned long maxlength);

#endif /* _freax_UCS2_STRING_H_ */
