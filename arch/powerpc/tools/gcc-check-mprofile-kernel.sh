#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

set -e
set -o pipefail

# To debug, uncomment the following line
# set -x

# -mprofile-kernel is only supported on 64-bit with ELFv2, so this should not
# be invoked for other targets. Therefore we can pass in -m64 and -mabi
# explicitly, to take care of toolchains defaulting to other targets.

# Test whether the compile option -mprofile-kernel exists and generates
# profiling code (ie. a call to _mcount()).
echo "int func() { return 0; }" | \
    $* -m64 -mabi=elfv2 -S -x c -O2 -p -mprofile-kernel - -o - \
    2> /dev/null | grep -q "_mcount"

# Test whether the notrace attribute correctly suppresses calls to _mcount().

echo -e "#include <freax/compiler.h>\nnotrace int func() { return 0; }" | \
    $* -m64 -mabi=elfv2 -S -x c -O2 -p -mprofile-kernel - -o - \
    2> /dev/null | grep -q "_mcount" && \
    exit 1

exit 0
