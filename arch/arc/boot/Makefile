# SPDX-License-Identifier: GPL-2.0

# uImage build relies on mkimage being availble on your host for ARC target
# You will need to build u-boot for ARC, rename mkimage to arc-elf32-mkimage
# and make sure it's reacable from your PATH

OBJCOPYFLAGS= -O binary -R .note -R .note.gnu.build-id -R .comment -S

freax_START_TEXT = $$($(READELF) -h vmfreax | \
			grep "Entry point address" | grep -o 0x.*)

UIMAGE_LOADADDR    = $(CONFIG_freax_LINK_BASE)
UIMAGE_ENTRYADDR   = $(freax_START_TEXT)

targets += vmfreax.bin
targets += vmfreax.bin.gz
targets += vmfreax.bin.lzma
targets += uImage.bin
targets += uImage.gz
targets += uImage.lzma

$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/vmfreax.bin.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)

$(obj)/vmfreax.bin.lzma: $(obj)/vmfreax.bin FORCE
	$(call if_changed,lzma)

$(obj)/uImage.bin: $(obj)/vmfreax.bin FORCE
	$(call if_changed,uimage,none)

$(obj)/uImage.gz: $(obj)/vmfreax.bin.gz FORCE
	$(call if_changed,uimage,gzip)

$(obj)/uImage.lzma: $(obj)/vmfreax.bin.lzma FORCE
	$(call if_changed,uimage,lzma)
