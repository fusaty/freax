#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# Generate atomic headers

ATOMICDIR=$(dirname $0)
ATOMICTBL=${ATOMICDIR}/atomics.tbl
freaxDIR=${ATOMICDIR}/../..

cat <<EOF |
gen-atomic-instrumented.sh      freax/atomic/atomic-instrumented.h
gen-atomic-long.sh              freax/atomic/atomic-long.h
gen-atomic-fallback.sh          freax/atomic/atomic-arch-fallback.h
EOF
while read script header args; do
	/bin/sh ${ATOMICDIR}/${script} ${ATOMICTBL} ${args} > ${freaxDIR}/include/${header}
	HASH="$(sha1sum ${freaxDIR}/include/${header})"
	HASH="${HASH%% *}"
	printf "// %s\n" "${HASH}" >> ${freaxDIR}/include/${header}
done
