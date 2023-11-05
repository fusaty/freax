#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1

if [ $# -ne 1 ] ; then
	freax_header_dir=tools/include/uapi/freax
else
	freax_header_dir=$1
fi

freax_mount=${freax_header_dir}/mount.h

printf "static const char *fspick_flags[] = {\n"
regex='^[[:space:]]*#[[:space:]]*define[[:space:]]+FSPICK_([[:alnum:]_]+)[[:space:]]+(0x[[:xdigit:]]+)[[:space:]]*.*'
grep -E $regex ${freax_mount} | \
	sed -r "s/$regex/\2 \1/g"	| \
	xargs printf "\t[ilog2(%s) + 1] = \"%s\",\n"
printf "};\n"
