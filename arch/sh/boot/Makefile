#
# arch/sh/boot/Makefile
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1999 Stuart Menefy
#

#
# Assign safe dummy values if these variables are not defined,
# in order to suppress error message.
#
CONFIG_PAGE_OFFSET	?= 0x80000000
CONFIG_MEMORY_START	?= 0x0c000000
CONFIG_BOOT_LINK_OFFSET	?= 0x00800000
CONFIG_ZERO_PAGE_OFFSET	?= 0x00001000
CONFIG_ENTRY_OFFSET	?= 0x00001000
CONFIG_PHYSICAL_START	?= $(CONFIG_MEMORY_START)

suffix_y := bin
suffix_$(CONFIG_KERNEL_GZIP)	:= gz
suffix_$(CONFIG_KERNEL_BZIP2)	:= bz2
suffix_$(CONFIG_KERNEL_LZMA)	:= lzma
suffix_$(CONFIG_KERNEL_XZ)	:= xz
suffix_$(CONFIG_KERNEL_LZO)	:= lzo

targets := zImage vmfreax.srec romImage uImage uImage.srec uImage.gz \
	   uImage.bz2 uImage.lzma uImage.xz uImage.lzo uImage.bin \
	   vmfreax.bin vmfreax.bin.gz vmfreax.bin.bz2 vmfreax.bin.lzma \
	   vmfreax.bin.xz vmfreax.bin.lzo
subdir- := compressed romimage

$(obj)/zImage: $(obj)/compressed/vmfreax FORCE
	$(call if_changed,objcopy)
	@echo '  Kernel: $@ is ready'

$(obj)/compressed/vmfreax: FORCE
	$(Q)$(MAKE) $(build)=$(obj)/compressed $@

$(obj)/romImage: $(obj)/romimage/vmfreax FORCE
	$(call if_changed,objcopy)
	@echo '  Kernel: $@ is ready'

$(obj)/romimage/vmfreax: $(obj)/zImage FORCE
	$(Q)$(MAKE) $(build)=$(obj)/romimage $@

KERNEL_MEMORY	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_PHYSICAL_START) & 0x1fffffff]')

KERNEL_LOAD	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_PAGE_OFFSET)  + \
			$(KERNEL_MEMORY) + \
			$(CONFIG_ZERO_PAGE_OFFSET)]')

KERNEL_ENTRY	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_PAGE_OFFSET)  + \
			$(KERNEL_MEMORY) + \
			$(CONFIG_ZERO_PAGE_OFFSET) + $(CONFIG_ENTRY_OFFSET)]')

UIMAGE_LOADADDR = $(KERNEL_LOAD)
UIMAGE_ENTRYADDR = $(KERNEL_ENTRY)

$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/vmfreax.bin.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)

$(obj)/vmfreax.bin.bz2: $(obj)/vmfreax.bin FORCE
	$(call if_changed,bzip2)

$(obj)/vmfreax.bin.lzma: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzma)

$(obj)/vmfreax.bin.xz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,xzkern)

$(obj)/vmfreax.bin.lzo: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzo)

$(obj)/uImage.bz2: $(obj)/vmfreax.bin.bz2 FORCE
	$(call if_changed,uimage,bzip2)

$(obj)/uImage.gz: $(obj)/vmfreax.bin.gz FORCE
	$(call if_changed,uimage,gzip)

$(obj)/uImage.lzma: $(obj)/vmfreax.bin.lzma FORCE
	$(call if_changed,uimage,lzma)

$(obj)/uImage.xz: $(obj)/vmfreax.bin.xz FORCE
	$(call if_changed,uimage,xz)

$(obj)/uImage.lzo: $(obj)/vmfreax.bin.lzo FORCE
	$(call if_changed,uimage,lzo)

$(obj)/uImage.bin: $(obj)/vmfreax.bin FORCE
	$(call if_changed,uimage,none)

OBJCOPYFLAGS_vmfreax.srec := -I binary -O srec
$(obj)/vmfreax.srec: $(obj)/compressed/vmfreax FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_uImage.srec := -I binary -O srec
$(obj)/uImage.srec: $(obj)/uImage FORCE
	$(call if_changed,objcopy)

$(obj)/uImage: $(obj)/uImage.$(suffix_y)
	@ln -sf $(notdir $<) $@
	@echo '  Image $@ is ready'

export CONFIG_PAGE_OFFSET CONFIG_MEMORY_START CONFIG_BOOT_LINK_OFFSET \
       CONFIG_PHYSICAL_START CONFIG_ZERO_PAGE_OFFSET CONFIG_ENTRY_OFFSET \
       KERNEL_MEMORY suffix_y
