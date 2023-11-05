#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1

if [ $# -ne 1 ] ; then
	freax_header_dir=tools/include/uapi/freax
else
	freax_header_dir=$1
fi

freax_mount=${freax_header_dir}/mount.h

printf "static const char *fsconfig_cmds[] = {\n"
ms='[[:space:]]*'
sed -nr "s/^${ms}FSCONFIG_([[:alnum:]_]+)${ms}=${ms}([[:digit:]]+)${ms},.*/\t[\2] = \"\1\",/p" \
	${freax_mount}
printf "};\n"
