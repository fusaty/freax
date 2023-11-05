#
# arch/nios2/boot/Makefile
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#

UIMAGE_LOADADDR = $(shell $(NM) vmfreax | awk '$$NF == "_stext" {print $$1}')
UIMAGE_ENTRYADDR = $(shell $(NM) vmfreax | awk '$$NF == "_start" {print $$1}')
UIMAGE_COMPRESSION = gzip

OBJCOPYFLAGS_vmfreax.bin := -O binary

targets += vmfreax.bin vmfreax.gz vmImage

$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/vmfreax.gz: $(obj)/vmfreax.bin FORCE
	$(call if_changed,gzip)

$(obj)/vmImage: $(obj)/vmfreax.gz FORCE
	$(call if_changed,uimage)
	@$(kecho) 'Kernel: $@ is ready'

$(obj)/zImage: $(obj)/compressed/vmfreax FORCE
	$(call if_changed,objcopy)
	@$(kecho) 'Kernel: $@ is ready'

$(obj)/compressed/vmfreax: $(obj)/vmfreax.gz FORCE
	$(Q)$(MAKE) $(build)=$(obj)/compressed $@
