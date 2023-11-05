// SPDX-License-Identifier: GPL-2.0

#include <freax/ctype.h>
#include <freax/string.h>
#include <freax/types.h>

char *skip_spaces(const char *str)
{
	while (isspace(*str))
		++str;
	return (char *)str;
}
