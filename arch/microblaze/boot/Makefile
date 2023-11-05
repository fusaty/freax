# SPDX-License-Identifier: GPL-2.0
#
# arch/microblaze/boot/Makefile
#

targets := freax.bin freax.bin.gz freax.bin.ub simpleImage.*

OBJCOPYFLAGS := -R .note -R .comment -R .note.gnu.build-id -O binary

$(obj)/freax.bin: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/freax.bin.ub: $(obj)/freax.bin FORCE
	$(call if_changed,uimage)

$(obj)/freax.bin.gz: $(obj)/freax.bin FORCE
	$(call if_changed,gzip)

quiet_cmd_strip = STRIP   $< $@$2
	cmd_strip = $(STRIP) -K microblaze_start -K _end -K __log_buf \
				-K _fdt_start $< -o $@$2

UIMAGE_LOADADDR = $(CONFIG_KERNEL_BASE_ADDR)

$(obj)/simpleImage.$(DTB): vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/simpleImage.$(DTB).ub: $(obj)/simpleImage.$(DTB) FORCE
	$(call if_changed,uimage)

$(obj)/simpleImage.$(DTB).unstrip: vmfreax FORCE
	$(call if_changed,copy)

$(obj)/simpleImage.$(DTB).strip: vmfreax FORCE
	$(call if_changed,strip)
