# SPDX-License-Identifier: GPL-2.0
#
# freax/arch/x86/boot/compressed/Makefile
#
# create a compressed vmfreax image from the original vmfreax
#
# vmlinuz is:
#	decompression code (*.o)
#	asm globals (piggy.S), including:
#		vmfreax.bin.(gz|bz2|lzma|...)
#
# vmfreax.bin is:
#	vmfreax stripped of debugging and comments
# vmfreax.bin.all is:
#	vmfreax.bin + vmfreax.relocs
# vmfreax.bin.(gz|bz2|lzma|...) is:
#	(see scripts/Makefile.lib size_append)
#	compressed vmfreax.bin.all + u32 size of vmfreax.bin.all

# Sanitizer runtimes are unavailable and cannot be linked for early boot code.
KASAN_SANITIZE			:= n
KCSAN_SANITIZE			:= n
KMSAN_SANITIZE			:= n
OBJECT_FILES_NON_STANDARD	:= y

# Prevents link failures: __sanitizer_cov_trace_pc() is not linked in.
KCOV_INSTRUMENT		:= n

targets := vmfreax vmfreax.bin vmfreax.bin.gz vmfreax.bin.bz2 vmfreax.bin.lzma \
	vmfreax.bin.xz vmfreax.bin.lzo vmfreax.bin.lz4 vmfreax.bin.zst

# CLANG_FLAGS must come before any cc-disable-warning or cc-option calls in
# case of cross compiling, as it has the '--target=' flag, which is needed to
# avoid errors with '-march=i386', and future flags may depend on the target to
# be valid.
KBUILD_CFLAGS := -m$(BITS) -O2 $(CLANG_FLAGS)
KBUILD_CFLAGS += -fno-strict-aliasing -fPIE
KBUILD_CFLAGS += -Wundef
KBUILD_CFLAGS += -DDISABLE_BRANCH_PROFILING
cflags-$(CONFIG_X86_32) := -march=i386
cflags-$(CONFIG_X86_64) := -mcmodel=small -mno-red-zone
KBUILD_CFLAGS += $(cflags-y)
KBUILD_CFLAGS += -mno-mmx -mno-sse
KBUILD_CFLAGS += -ffreestanding -fshort-wchar
KBUILD_CFLAGS += -fno-stack-protector
KBUILD_CFLAGS += $(call cc-disable-warning, address-of-packed-member)
KBUILD_CFLAGS += $(call cc-disable-warning, gnu)
KBUILD_CFLAGS += -Wno-pointer-sign
KBUILD_CFLAGS += $(call cc-option,-fmacro-prefix-map=$(srctree)/=)
KBUILD_CFLAGS += -fno-asynchronous-unwind-tables
KBUILD_CFLAGS += -D__DISABLE_EXPORTS
# Disable relocation relaxation in case the link is not PIE.
KBUILD_CFLAGS += $(call cc-option,-Wa$(comma)-mrelax-relocations=no)
KBUILD_CFLAGS += -include $(srctree)/include/freax/hidden.h

# sev.c indirectly inludes inat-table.h which is generated during
# compilation and stored in $(objtree). Add the directory to the includes so
# that the compiler finds it even with out-of-tree builds (make O=/some/path).
CFLAGS_sev.o += -I$(objtree)/arch/x86/lib/

KBUILD_AFLAGS  := $(KBUILD_CFLAGS) -D__ASSEMBLY__
GCOV_PROFILE := n
UBSAN_SANITIZE :=n

KBUILD_LDFLAGS := -m elf_$(UTS_MACHINE)
KBUILD_LDFLAGS += $(call ld-option,--no-ld-generated-unwind-info)
# Compressed kernel should be built as PIE since it may be loaded at any
# address by the bootloader.
LDFLAGS_vmfreax := -pie $(call ld-option, --no-dynamic-linker)
ifdef CONFIG_LD_ORPHAN_WARN
LDFLAGS_vmfreax += --orphan-handling=$(CONFIG_LD_ORPHAN_WARN_LEVEL)
endif
LDFLAGS_vmfreax += -z noexecstack
ifeq ($(CONFIG_LD_IS_BFD),y)
LDFLAGS_vmfreax += $(call ld-option,--no-warn-rwx-segments)
endif
ifeq ($(CONFIG_EFI_STUB),y)
# ensure that the static EFI stub library will be pulled in, even if it is
# never referenced explicitly from the startup code
LDFLAGS_vmfreax += -u efi_pe_entry
endif
LDFLAGS_vmfreax += -T

hostprogs	:= mkpiggy
HOST_EXTRACFLAGS += -I$(srctree)/tools/include

sed-voffset := -e 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(_text\|__bss_start\|_end\)$$/\#define VO_\2 _AC(0x\1,UL)/p'

quiet_cmd_voffset = VOFFSET $@
      cmd_voffset = $(NM) $< | sed -n $(sed-voffset) > $@

targets += ../voffset.h

$(obj)/../voffset.h: vmfreax FORCE
	$(call if_changed,voffset)

$(obj)/misc.o: $(obj)/../voffset.h

vmfreax-objs-y := $(obj)/vmfreax.lds $(obj)/kernel_info.o $(obj)/head_$(BITS).o \
	$(obj)/misc.o $(obj)/string.o $(obj)/cmdline.o $(obj)/error.o \
	$(obj)/piggy.o $(obj)/cpuflags.o

vmfreax-objs-$(CONFIG_EARLY_PRINTK) += $(obj)/early_serial_console.o
vmfreax-objs-$(CONFIG_RANDOMIZE_BASE) += $(obj)/kaslr.o
ifdef CONFIG_X86_64
	vmfreax-objs-y += $(obj)/ident_map_64.o
	vmfreax-objs-y += $(obj)/idt_64.o $(obj)/idt_handlers_64.o
	vmfreax-objs-$(CONFIG_AMD_MEM_ENCRYPT) += $(obj)/mem_encrypt.o
	vmfreax-objs-y += $(obj)/pgtable_64.o
	vmfreax-objs-$(CONFIG_AMD_MEM_ENCRYPT) += $(obj)/sev.o
endif

vmfreax-objs-$(CONFIG_ACPI) += $(obj)/acpi.o
vmfreax-objs-$(CONFIG_INTEL_TDX_GUEST) += $(obj)/tdx.o $(obj)/tdcall.o $(obj)/tdx-shared.o
vmfreax-objs-$(CONFIG_UNACCEPTED_MEMORY) += $(obj)/mem.o

vmfreax-objs-$(CONFIG_EFI) += $(obj)/efi.o
vmfreax-objs-$(CONFIG_EFI_MIXED) += $(obj)/efi_mixed.o
vmfreax-objs-$(CONFIG_EFI_STUB) += $(objtree)/drivers/firmware/efi/libstub/lib.a

$(obj)/vmfreax: $(vmfreax-objs-y) FORCE
	$(call if_changed,ld)

OBJCOPYFLAGS_vmfreax.bin :=  -R .comment -S
$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

targets += $(patsubst $(obj)/%,%,$(vmfreax-objs-y)) vmfreax.bin.all vmfreax.relocs

# vmfreax.relocs is created by the vmfreax postlink step.
$(obj)/vmfreax.relocs: vmfreax
	@true

vmfreax.bin.all-y := $(obj)/vmfreax.bin
vmfreax.bin.all-$(CONFIG_X86_NEED_RELOCS) += $(obj)/vmfreax.relocs

$(obj)/vmfreax.bin.gz: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,gzip)
$(obj)/vmfreax.bin.bz2: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,bzip2_with_size)
$(obj)/vmfreax.bin.lzma: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,lzma_with_size)
$(obj)/vmfreax.bin.xz: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,xzkern_with_size)
$(obj)/vmfreax.bin.lzo: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,lzo_with_size)
$(obj)/vmfreax.bin.lz4: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,lz4_with_size)
$(obj)/vmfreax.bin.zst: $(vmfreax.bin.all-y) FORCE
	$(call if_changed,zstd22_with_size)

suffix-$(CONFIG_KERNEL_GZIP)	:= gz
suffix-$(CONFIG_KERNEL_BZIP2)	:= bz2
suffix-$(CONFIG_KERNEL_LZMA)	:= lzma
suffix-$(CONFIG_KERNEL_XZ)	:= xz
suffix-$(CONFIG_KERNEL_LZO) 	:= lzo
suffix-$(CONFIG_KERNEL_LZ4) 	:= lz4
suffix-$(CONFIG_KERNEL_ZSTD)	:= zst

quiet_cmd_mkpiggy = MKPIGGY $@
      cmd_mkpiggy = $(obj)/mkpiggy $< > $@

targets += piggy.S
$(obj)/piggy.S: $(obj)/vmfreax.bin.$(suffix-y) $(obj)/mkpiggy FORCE
	$(call if_changed,mkpiggy)
