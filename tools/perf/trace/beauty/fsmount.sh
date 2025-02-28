#!/bin/sh
# SPDX-License-Identifier: LGPL-2.1

if [ $# -ne 1 ] ; then
	freax_header_dir=tools/include/uapi/freax
else
	freax_header_dir=$1
fi

freax_mount=${freax_header_dir}/mount.h

# Remove MOUNT_ATTR_RELATIME as it is zeros, handle it a special way in the beautifier
# Only handle MOUNT_ATTR_ followed by a capital letter/num as __ is special case
# for things like MOUNT_ATTR__ATIME that is a mask for the possible ATIME handling
# bits. Special case it as well in the beautifier

printf "static const char *fsmount_attr_flags[] = {\n"
regex='^[[:space:]]*#[[:space:]]*define[[:space:]]+MOUNT_ATTR_([[:alnum:]][[:alnum:]_]+)[[:space:]]+(0x[[:xdigit:]]+)[[:space:]]*.*'
grep -E $regex ${freax_mount} | grep -v MOUNT_ATTR_RELATIME | \
	sed -r "s/$regex/\2 \1/g"	| \
	xargs printf "\t[ilog2(%s) + 1] = \"%s\",\n"
printf "};\n"
