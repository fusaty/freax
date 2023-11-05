# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for the freax parisc-specific parts of the boot image creator.
#

targets := image
targets += bzImage
subdir- := compressed

$(obj)/image: vmfreax FORCE
	$(call if_changed,objcopy)

$(obj)/bzImage: $(if $(CONFIG_KERNEL_UNCOMPRESSED),$(objtree)/vmfreax,$(obj)/compressed/vmfreax) FORCE
	$(call if_changed,objcopy)

$(obj)/compressed/vmfreax: FORCE
	$(Q)$(MAKE) $(build)=$(obj)/compressed $@
