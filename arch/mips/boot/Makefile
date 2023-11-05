#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995, 1998, 2001, 2002 by Ralf Baechle
# Copyright (C) 2004  Maciej W. Rozycki
#

#
# Some DECstations need all possible sections of an ECOFF executable
#
ifdef CONFIG_MACH_DECSTATION
  e2eflag := -a
endif

#
# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
#
drop-sections := .reginfo .mdebug .comment .note .pdr .options .MIPS.options
strip-flags   := $(addprefix --remove-section=,$(drop-sections))

hostprogs := elf2ecoff

suffix-y			:= bin
suffix-$(CONFIG_KERNEL_BZIP2)	:= bz2
suffix-$(CONFIG_KERNEL_GZIP)	:= gz
suffix-$(CONFIG_KERNEL_LZMA)	:= lzma
suffix-$(CONFIG_KERNEL_LZO)	:= lzo

targets := vmfreax.ecoff
quiet_cmd_ecoff = ECOFF	  $@
      cmd_ecoff = $(obj)/elf2ecoff $(VMfreax) $@ $(e2eflag)
$(obj)/vmfreax.ecoff: $(obj)/elf2ecoff $(VMfreax) FORCE
	$(call if_changed,ecoff)

targets += vmfreax.bin
quiet_cmd_bin = OBJCOPY $@
      cmd_bin = $(OBJCOPY) -O binary $(strip-flags) $(VMfreax) $@
$(obj)/vmfreax.bin: $(VMfreax) FORCE
	$(call if_changed,bin)

targets += vmfreax.srec
quiet_cmd_srec = OBJCOPY $@
      cmd_srec = $(OBJCOPY) -S -O srec $(strip-flags) $(VMfreax) $@
$(obj)/vmfreax.srec: $(VMfreax) FORCE
	$(call if_changed,srec)

UIMAGE_LOADADDR  = $(VMfreax_LOAD_ADDRESS)
UIMAGE_ENTRYADDR = $(VMfreax_ENTRY_ADDRESS)

#
# Compressed vmfreax images
#

extra-y += vmfreax.bin.bz2
extra-y += vmfreax.bin.gz
extra-y += vmfreax.bin.lzma
extra-y += vmfreax.bin.lzo

$(obj)/vmfreax.bin.bz2: $(obj)/vmfreax.bin FORCE
	$(call if_changed,bzip2)

$(obj)/vmfreax.bin.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)

$(obj)/vmfreax.bin.lzma: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzma)

$(obj)/vmfreax.bin.lzo: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzo)

#
# Compressed u-boot images
#

targets += uImage
targets += uImage.bin
targets += uImage.bz2
targets += uImage.gz
targets += uImage.lzma
targets += uImage.lzo

$(obj)/uImage.bin: $(obj)/vmfreax.bin FORCE
	$(call if_changed,uimage,none)

$(obj)/uImage.bz2: $(obj)/vmfreax.bin.bz2 FORCE
	$(call if_changed,uimage,bzip2)

$(obj)/uImage.gz: $(obj)/vmfreax.bin.gz FORCE
	$(call if_changed,uimage,gzip)

$(obj)/uImage.lzma: $(obj)/vmfreax.bin.lzma FORCE
	$(call if_changed,uimage,lzma)

$(obj)/uImage.lzo: $(obj)/vmfreax.bin.lzo FORCE
	$(call if_changed,uimage,lzo)

$(obj)/uImage: $(obj)/uImage.$(suffix-y)
	@ln -sf $(notdir $<) $@
	@echo '  Image $@ is ready'

#
# Flattened Image Tree (.itb) images
#

ifeq ($(ADDR_BITS),32)
itb_addr_cells = 1
endif
ifeq ($(ADDR_BITS),64)
itb_addr_cells = 2
endif

targets += vmfreax.its.S

quiet_cmd_its_cat = CAT     $@
      cmd_its_cat = cat $(real-prereqs) >$@

$(obj)/vmfreax.its.S: $(addprefix $(srctree)/arch/mips/$(PLATFORM)/,$(ITS_INPUTS)) FORCE
	$(call if_changed,its_cat)

targets += vmfreax.its
targets += vmfreax.gz.its
targets += vmfreax.bz2.its
targets += vmfreax.lzma.its
targets += vmfreax.lzo.its

quiet_cmd_cpp_its_S = ITS     $@
      cmd_cpp_its_S = $(CPP) -P -C -o $@ $< \
		        -DKERNEL_NAME="\"freax $(KERNELRELEASE)\"" \
			-DVMfreax_BINARY="\"$(3)\"" \
			-DVMfreax_COMPRESSION="\"$(2)\"" \
			-DVMfreax_LOAD_ADDRESS=$(VMfreax_LOAD_ADDRESS) \
			-DVMfreax_ENTRY_ADDRESS=$(VMfreax_ENTRY_ADDRESS) \
			-DADDR_BITS=$(ADDR_BITS) \
			-DADDR_CELLS=$(itb_addr_cells)

$(obj)/vmfreax.its: $(obj)/vmfreax.its.S $(VMfreax) FORCE
	$(call if_changed,cpp_its_S,none,vmfreax.bin)

$(obj)/vmfreax.gz.its: $(obj)/vmfreax.its.S $(VMfreax) FORCE
	$(call if_changed,cpp_its_S,gzip,vmfreax.bin.gz)

$(obj)/vmfreax.bz2.its: $(obj)/vmfreax.its.S $(VMfreax)  FORCE
	$(call if_changed,cpp_its_S,bzip2,vmfreax.bin.bz2)

$(obj)/vmfreax.lzma.its: $(obj)/vmfreax.its.S $(VMfreax) FORCE
	$(call if_changed,cpp_its_S,lzma,vmfreax.bin.lzma)

$(obj)/vmfreax.lzo.its: $(obj)/vmfreax.its.S $(VMfreax) FORCE
	$(call if_changed,cpp_its_S,lzo,vmfreax.bin.lzo)

targets += vmfreax.itb
targets += vmfreax.gz.itb
targets += vmfreax.bz2.itb
targets += vmfreax.lzma.itb
targets += vmfreax.lzo.itb

quiet_cmd_itb-image = ITB     $@
      cmd_itb-image = \
		env PATH="$(objtree)/scripts/dtc:$(PATH)" \
		$(BASH) $(MKIMAGE) \
		-D "-I dts -O dtb -p 500 \
			--include $(objtree)/arch/mips \
			--warning no-unit_address_vs_reg" \
		-f $(2) $@

$(obj)/vmfreax.itb: $(obj)/vmfreax.its $(obj)/vmfreax.bin FORCE
	$(call if_changed,itb-image,$<)

$(obj)/vmfreax.%.itb: $(obj)/vmfreax.%.its $(obj)/vmfreax.bin.% FORCE
	$(call if_changed,itb-image,$<)

# for cleaning
subdir- += compressed tools
