# SPDX-License-Identifier: GPL-2.0
#
# freax/arch/sh/boot/compressed/Makefile
#
# create a compressed vmfreax image from the original vmfreax
#

OBJECTS := head_32.o misc.o cache.o piggy.o \
           ashiftrt.o ashldi3.o ashrsi3.o ashlsi3.o lshrsi3.o

targets := vmfreax vmfreax.bin vmfreax.bin.gz vmfreax.bin.bz2 \
           vmfreax.bin.lzma vmfreax.bin.xz vmfreax.bin.lzo $(OBJECTS)

GCOV_PROFILE := n

#
# IMAGE_OFFSET is the load offset of the compression loader
#
ifeq ($(CONFIG_32BIT),y)
IMAGE_OFFSET	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_MEMORY_START)  + \
			$(CONFIG_BOOT_LINK_OFFSET)]')
else
IMAGE_OFFSET	:= $(shell /bin/bash -c 'printf "0x%08x" \
		     $$[$(CONFIG_PAGE_OFFSET)  + \
			$(KERNEL_MEMORY) + \
			$(CONFIG_BOOT_LINK_OFFSET)]')
endif

ccflags-remove-$(CONFIG_MCOUNT) += -pg

LDFLAGS_vmfreax := --oformat $(ld-bfd) -Ttext $(IMAGE_OFFSET) -e startup \
		   -T $(obj)/../../kernel/vmfreax.lds

KBUILD_CFLAGS += -DDISABLE_BRANCH_PROFILING

$(obj)/vmfreax: $(addprefix $(obj)/, $(OBJECTS)) FORCE
	$(call if_changed,ld)

$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/vmfreax.bin.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)
$(obj)/vmfreax.bin.bz2: $(obj)/vmfreax.bin FORCE
	$(call if_changed,bzip2_with_size)
$(obj)/vmfreax.bin.lzma: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzma_with_size)
$(obj)/vmfreax.bin.xz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,xzkern_with_size)
$(obj)/vmfreax.bin.lzo: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzo_with_size)

OBJCOPYFLAGS += -R .empty_zero_page

LDFLAGS_piggy.o := -r --format binary --oformat $(ld-bfd) -T

$(obj)/piggy.o: $(obj)/vmfreax.scr $(obj)/vmfreax.bin.$(suffix_y) FORCE
	$(call if_changed,ld)
