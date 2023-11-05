#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1

if [ $# -ne 1 ] ; then
	freax_header_dir=tools/include/uapi/freax
else
	freax_header_dir=$1
fi

freax_mman=${freax_header_dir}/mman.h

printf "static const char *mremap_flags[] = {\n"
regex='^[[:space:]]*#[[:space:]]*define[[:space:]]+MREMAP_([[:alnum:]_]+)[[:space:]]+((0x)?[[:xdigit:]]+)[[:space:]]*.*'
grep -E -q $regex ${freax_mman} && \
(grep -E $regex ${freax_mman} | \
	sed -r "s/$regex/\2 \1 \1 \1 \2/g"	| \
	xargs printf "\t[ilog2(%s) + 1] = \"%s\",\n#ifndef MREMAP_%s\n#define MREMAP_%s %s\n#endif\n")
printf "};\n"
