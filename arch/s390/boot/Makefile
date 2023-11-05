# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the freax s390-specific parts of the memory manager.
#

KCOV_INSTRUMENT := n
GCOV_PROFILE := n
UBSAN_SANITIZE := n
KASAN_SANITIZE := n
KCSAN_SANITIZE := n

KBUILD_AFLAGS := $(KBUILD_AFLAGS_DECOMPRESSOR)
KBUILD_CFLAGS := $(KBUILD_CFLAGS_DECOMPRESSOR)

#
# Use minimum architecture for als.c to be able to print an error
# message if the kernel is started on a machine which is too old
#
ifndef CONFIG_CC_IS_CLANG
CC_FLAGS_MARCH_MINIMUM := -march=z900
else
CC_FLAGS_MARCH_MINIMUM := -march=z10
endif

ifneq ($(CC_FLAGS_MARCH),$(CC_FLAGS_MARCH_MINIMUM))
AFLAGS_REMOVE_head.o		+= $(CC_FLAGS_MARCH)
AFLAGS_head.o			+= $(CC_FLAGS_MARCH_MINIMUM)
AFLAGS_REMOVE_mem.o		+= $(CC_FLAGS_MARCH)
AFLAGS_mem.o			+= $(CC_FLAGS_MARCH_MINIMUM)
CFLAGS_REMOVE_als.o		+= $(CC_FLAGS_MARCH)
CFLAGS_als.o			+= $(CC_FLAGS_MARCH_MINIMUM)
CFLAGS_REMOVE_sclp_early_core.o	+= $(CC_FLAGS_MARCH)
CFLAGS_sclp_early_core.o	+= $(CC_FLAGS_MARCH_MINIMUM)
endif

CFLAGS_sclp_early_core.o += -I$(srctree)/drivers/s390/char

obj-y	:= head.o als.o startup.o physmem_info.o ipl_parm.o ipl_report.o vmem.o
obj-y	+= string.o ebcdic.o sclp_early_core.o mem.o ipl_vmparm.o cmdline.o
obj-y	+= version.o pgm_check_info.o ctype.o ipl_data.o machine_kexec_reloc.o
obj-$(findstring y, $(CONFIG_PROTECTED_VIRTUALIZATION_GUEST) $(CONFIG_PGSTE))	+= uv.o
obj-$(CONFIG_RANDOMIZE_BASE)	+= kaslr.o
obj-y	+= $(if $(CONFIG_KERNEL_UNCOMPRESSED),,decompressor.o) info.o
obj-$(CONFIG_KERNEL_ZSTD) += clz_ctz.o
obj-all := $(obj-y) piggy.o syms.o

targets	:= bzImage section_cmp.boot.data section_cmp.boot.preserved.data $(obj-y)
targets	+= vmfreax.lds vmfreax vmfreax.bin vmfreax.bin.gz vmfreax.bin.bz2
targets += vmfreax.bin.xz vmfreax.bin.lzma vmfreax.bin.lzo vmfreax.bin.lz4
targets += vmfreax.bin.zst info.bin syms.bin vmfreax.syms $(obj-all)

OBJECTS := $(addprefix $(obj)/,$(obj-y))
OBJECTS_ALL := $(addprefix $(obj)/,$(obj-all))

clean-files += vmfreax.map

quiet_cmd_section_cmp = SECTCMP $*
define cmd_section_cmp
	s1=`$(OBJDUMP) -t -j "$*" "$<" | sort | \
		sed -n "/0000000000000000/! s/.*\s$*\s\+//p" | sha256sum`; \
	s2=`$(OBJDUMP) -t -j "$*" "$(word 2,$^)" | sort | \
		sed -n "/0000000000000000/! s/.*\s$*\s\+//p" | sha256sum`; \
	if [ "$$s1" != "$$s2" ]; then \
		echo "error: section $* differs between $< and $(word 2,$^)" >&2; \
		exit 1; \
	fi; \
	touch $@
endef

$(obj)/bzImage: $(obj)/vmfreax $(obj)/section_cmp.boot.data $(obj)/section_cmp.boot.preserved.data FORCE
	$(call if_changed,objcopy)

$(obj)/section_cmp%: vmfreax $(obj)/vmfreax FORCE
	$(call if_changed,section_cmp)

LDFLAGS_vmfreax := --oformat $(LD_BFD) -e startup $(if $(CONFIG_VMfreax_MAP),-Map=$(obj)/vmfreax.map) --build-id=sha1 -T
$(obj)/vmfreax: $(obj)/vmfreax.lds $(OBJECTS_ALL) FORCE
	$(call if_changed,ld)

LDFLAGS_vmfreax.syms := --oformat $(LD_BFD) -e startup -T
$(obj)/vmfreax.syms: $(obj)/vmfreax.lds $(OBJECTS) FORCE
	$(call if_changed,ld)

quiet_cmd_dumpsyms = DUMPSYMS $<
define cmd_dumpsyms
	$(NM) -n -S --format=bsd "$<" | sed -nE 's/^0*([0-9a-fA-F]+) 0*([0-9a-fA-F]+) [tT] ([^ ]*)$$/\1 \2 \3/p' | tr '\n' '\0' > "$@"
endef

$(obj)/syms.bin: $(obj)/vmfreax.syms FORCE
	$(call if_changed,dumpsyms)

OBJCOPYFLAGS_syms.o := -I binary -O elf64-s390 -B s390:64-bit --rename-section .data=.decompressor.syms
$(obj)/syms.o: $(obj)/syms.bin FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_info.bin := -O binary --only-section=.vmfreax.info --set-section-flags .vmfreax.info=load
$(obj)/info.bin: vmfreax FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_info.o := -I binary -O elf64-s390 -B s390:64-bit --rename-section .data=.vmfreax.info
$(obj)/info.o: $(obj)/info.bin FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_vmfreax.bin := -O binary --remove-section=.comment --remove-section=.vmfreax.info -S
$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

suffix-$(CONFIG_KERNEL_GZIP)  := .gz
suffix-$(CONFIG_KERNEL_BZIP2) := .bz2
suffix-$(CONFIG_KERNEL_LZ4)  := .lz4
suffix-$(CONFIG_KERNEL_LZMA)  := .lzma
suffix-$(CONFIG_KERNEL_LZO)  := .lzo
suffix-$(CONFIG_KERNEL_XZ)  := .xz
suffix-$(CONFIG_KERNEL_ZSTD)  := .zst

$(obj)/vmfreax.bin.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)
$(obj)/vmfreax.bin.bz2: $(obj)/vmfreax.bin FORCE
	$(call if_changed,bzip2_with_size)
$(obj)/vmfreax.bin.lz4: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lz4_with_size)
$(obj)/vmfreax.bin.lzma: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzma_with_size)
$(obj)/vmfreax.bin.lzo: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzo_with_size)
$(obj)/vmfreax.bin.xz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,xzkern_with_size)
$(obj)/vmfreax.bin.zst: $(obj)/vmfreax.bin FORCE
	$(call if_changed,zstd22_with_size)

OBJCOPYFLAGS_piggy.o := -I binary -O elf64-s390 -B s390:64-bit --rename-section .data=.vmfreax.bin.compressed
$(obj)/piggy.o: $(obj)/vmfreax.bin$(suffix-y) FORCE
	$(call if_changed,objcopy)
