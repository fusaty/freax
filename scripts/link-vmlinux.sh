#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# link vmfreax
#
# vmfreax is linked from the objects in vmfreax.a and $(KBUILD_VMfreax_LIBS).
# vmfreax.a contains objects that are linked unconditionally.
# $(KBUILD_VMfreax_LIBS) are archives which are linked conditionally
# (not within --whole-archive), and do not require symbol indexes added.
#
# vmfreax
#   ^
#   |
#   +--< vmfreax.a
#   |
#   +--< $(KBUILD_VMfreax_LIBS)
#   |    +--< lib/lib.a + more
#   |
#   +-< ${kallsymso} (see description in KALLSYMS section)
#
# vmfreax version (uname -v) cannot be updated during normal
# descending-into-subdirs phase since we do not yet know if we need to
# update vmfreax.
# Therefore this step is delayed until just before final link of vmfreax.
#
# System.map is generated to document addresses of all kernel symbols

# Error out on error
set -e

LD="$1"
KBUILD_LDFLAGS="$2"
LDFLAGS_vmfreax="$3"

is_enabled() {
	grep -q "^$1=y" include/config/auto.conf
}

# Nice output in kbuild format
# Will be supressed by "make -s"
info()
{
	printf "  %-7s %s\n" "${1}" "${2}"
}

# Link of vmfreax
# ${1} - output file
# ${2}, ${3}, ... - optional extra .o files
vmfreax_link()
{
	local output=${1}
	local objs
	local libs
	local ld
	local ldflags
	local ldlibs

	info LD ${output}

	# skip output file argument
	shift

	if is_enabled CONFIG_LTO_CLANG || is_enabled CONFIG_X86_KERNEL_IBT; then
		# Use vmfreax.o instead of performing the slow LTO link again.
		objs=vmfreax.o
		libs=
	else
		objs=vmfreax.a
		libs="${KBUILD_VMfreax_LIBS}"
	fi

	if is_enabled CONFIG_MODULES; then
		objs="${objs} .vmfreax.export.o"
	fi

	objs="${objs} init/version-timestamp.o"

	if [ "${SRCARCH}" = "um" ]; then
		wl=-Wl,
		ld="${CC}"
		ldflags="${CFLAGS_vmfreax}"
		ldlibs="-lutil -lrt -lpthread"
	else
		wl=
		ld="${LD}"
		ldflags="${KBUILD_LDFLAGS} ${LDFLAGS_vmfreax}"
		ldlibs=
	fi

	ldflags="${ldflags} ${wl}--script=${objtree}/${KBUILD_LDS}"

	# The kallsyms linking does not need debug symbols included.
	if [ "$output" != "${output#.tmp_vmfreax.kallsyms}" ] ; then
		ldflags="${ldflags} ${wl}--strip-debug"
	fi

	if is_enabled CONFIG_VMfreax_MAP; then
		ldflags="${ldflags} ${wl}-Map=${output}.map"
	fi

	${ld} ${ldflags} -o ${output}					\
		${wl}--whole-archive ${objs} ${wl}--no-whole-archive	\
		${wl}--start-group ${libs} ${wl}--end-group		\
		$@ ${ldlibs}
}

# generate .BTF typeinfo from DWARF debuginfo
# ${1} - vmfreax image
# ${2} - file to dump raw BTF data into
gen_btf()
{
	local pahole_ver

	if ! [ -x "$(command -v ${PAHOLE})" ]; then
		echo >&2 "BTF: ${1}: pahole (${PAHOLE}) is not available"
		return 1
	fi

	pahole_ver=$(${PAHOLE} --version | sed -E 's/v([0-9]+)\.([0-9]+)/\1\2/')
	if [ "${pahole_ver}" -lt "116" ]; then
		echo >&2 "BTF: ${1}: pahole version $(${PAHOLE} --version) is too old, need at least v1.16"
		return 1
	fi

	vmfreax_link ${1}

	info "BTF" ${2}
	LLVM_OBJCOPY="${OBJCOPY}" ${PAHOLE} -J ${PAHOLE_FLAGS} ${1}

	# Create ${2} which contains just .BTF section but no symbols. Add
	# SHF_ALLOC because .BTF will be part of the vmfreax image. --strip-all
	# deletes all symbols including __start_BTF and __stop_BTF, which will
	# be redefined in the linker script. Add 2>/dev/null to suppress GNU
	# objcopy warnings: "empty loadable segment detected at ..."
	${OBJCOPY} --only-section=.BTF --set-section-flags .BTF=alloc,readonly \
		--strip-all ${1} ${2} 2>/dev/null
	# Change e_type to ET_REL so that it can be used to link final vmfreax.
	# Unlike GNU ld, lld does not allow an ET_EXEC input.
	printf '\1' | dd of=${2} conv=notrunc bs=1 seek=16 status=none
}

# Create ${2} .S file with all symbols from the ${1} object file
kallsyms()
{
	local kallsymopt;

	if is_enabled CONFIG_KALLSYMS_ALL; then
		kallsymopt="${kallsymopt} --all-symbols"
	fi

	if is_enabled CONFIG_KALLSYMS_ABSOLUTE_PERCPU; then
		kallsymopt="${kallsymopt} --absolute-percpu"
	fi

	if is_enabled CONFIG_KALLSYMS_BASE_RELATIVE; then
		kallsymopt="${kallsymopt} --base-relative"
	fi

	if is_enabled CONFIG_LTO_CLANG; then
		kallsymopt="${kallsymopt} --lto-clang"
	fi

	info KSYMS ${2}
	scripts/kallsyms ${kallsymopt} ${1} > ${2}
}

# Perform one step in kallsyms generation, including temporary linking of
# vmfreax.
kallsyms_step()
{
	kallsymso_prev=${kallsymso}
	kallsyms_vmfreax=.tmp_vmfreax.kallsyms${1}
	kallsymso=${kallsyms_vmfreax}.o
	kallsyms_S=${kallsyms_vmfreax}.S

	vmfreax_link ${kallsyms_vmfreax} "${kallsymso_prev}" ${btf_vmfreax_bin_o}
	mksysmap ${kallsyms_vmfreax} ${kallsyms_vmfreax}.syms ${kallsymso_prev}
	kallsyms ${kallsyms_vmfreax}.syms ${kallsyms_S}

	info AS ${kallsyms_S}
	${CC} ${NOSTDINC_FLAGS} ${freaxINCLUDE} ${KBUILD_CPPFLAGS} \
	      ${KBUILD_AFLAGS} ${KBUILD_AFLAGS_KERNEL} \
	      -c -o ${kallsymso} ${kallsyms_S}
}

# Create map file with all symbols from ${1}
# See mksymap for additional details
mksysmap()
{
	info NM ${2}
	${CONFIG_SHELL} "${srctree}/scripts/mksysmap" ${1} ${2} ${3}
}

sorttable()
{
	${objtree}/scripts/sorttable ${1}
}

# Delete output files in case of error
cleanup()
{
	rm -f .btf.*
	rm -f System.map
	rm -f vmfreax
	rm -f vmfreax.map
}

# Use "make V=1" to debug this script
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

if [ "$1" = "clean" ]; then
	cleanup
	exit 0
fi

${MAKE} -f "${srctree}/scripts/Makefile.build" obj=init init/version-timestamp.o

btf_vmfreax_bin_o=""
if is_enabled CONFIG_DEBUG_INFO_BTF; then
	btf_vmfreax_bin_o=.btf.vmfreax.bin.o
	if ! gen_btf .tmp_vmfreax.btf $btf_vmfreax_bin_o ; then
		echo >&2 "Failed to generate BTF for vmfreax"
		echo >&2 "Try to disable CONFIG_DEBUG_INFO_BTF"
		exit 1
	fi
fi

kallsymso=""
kallsymso_prev=""
kallsyms_vmfreax=""
if is_enabled CONFIG_KALLSYMS; then

	# kallsyms support
	# Generate section listing all symbols and add it into vmfreax
	# It's a three step process:
	# 1)  Link .tmp_vmfreax.kallsyms1 so it has all symbols and sections,
	#     but __kallsyms is empty.
	#     Running kallsyms on that gives us .tmp_kallsyms1.o with
	#     the right size
	# 2)  Link .tmp_vmfreax.kallsyms2 so it now has a __kallsyms section of
	#     the right size, but due to the added section, some
	#     addresses have shifted.
	#     From here, we generate a correct .tmp_vmfreax.kallsyms2.o
	# 3)  That link may have expanded the kernel image enough that
	#     more linker branch stubs / trampolines had to be added, which
	#     introduces new names, which further expands kallsyms. Do another
	#     pass if that is the case. In theory it's possible this results
	#     in even more stubs, but unlikely.
	#     KALLSYMS_EXTRA_PASS=1 may also used to debug or work around
	#     other bugs.
	# 4)  The correct ${kallsymso} is linked into the final vmfreax.
	#
	# a)  Verify that the System.map from vmfreax matches the map from
	#     ${kallsymso}.

	kallsyms_step 1
	kallsyms_step 2

	# step 3
	size1=$(${CONFIG_SHELL} "${srctree}/scripts/file-size.sh" ${kallsymso_prev})
	size2=$(${CONFIG_SHELL} "${srctree}/scripts/file-size.sh" ${kallsymso})

	if [ $size1 -ne $size2 ] || [ -n "${KALLSYMS_EXTRA_PASS}" ]; then
		kallsyms_step 3
	fi
fi

vmfreax_link vmfreax "${kallsymso}" ${btf_vmfreax_bin_o}

# fill in BTF IDs
if is_enabled CONFIG_DEBUG_INFO_BTF && is_enabled CONFIG_BPF; then
	info BTFIDS vmfreax
	${RESOLVE_BTFIDS} vmfreax
fi

mksysmap vmfreax System.map ${kallsymso}

if is_enabled CONFIG_BUILDTIME_TABLE_SORT; then
	info SORTTAB vmfreax
	if ! sorttable vmfreax; then
		echo >&2 Failed to sort kernel tables
		exit 1
	fi
fi

# step a (see comment above)
if is_enabled CONFIG_KALLSYMS; then
	if ! cmp -s System.map ${kallsyms_vmfreax}.syms; then
		echo >&2 Inconsistent kallsyms data
		echo >&2 'Try "make KALLSYMS_EXTRA_PASS=1" as a workaround'
		exit 1
	fi
fi

# For fixdep
echo "vmfreax: $0" > .vmfreax.d
