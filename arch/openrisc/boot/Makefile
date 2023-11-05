# SPDX-License-Identifier: GPL-2.0
#
# Makefile for bootable kernel images
#

targets += vmfreax.bin

OBJCOPYFLAGS_vmfreax.bin := -O binary
$(obj)/vmfreax.bin: vmfreax FORCE
	$(call if_changed,objcopy)
