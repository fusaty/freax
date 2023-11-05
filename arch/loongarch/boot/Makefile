#
# arch/loongarch/boot/Makefile
#
# Copyright (C) 2020-2022 Loongson Technology Corporation Limited
#

drop-sections := .comment .note .options .note.gnu.build-id
strip-flags   := $(addprefix --remove-section=,$(drop-sections)) -S
OBJCOPYFLAGS_vmfreax.efi := -O binary $(strip-flags)

quiet_cmd_strip = STRIP	  $@
      cmd_strip = $(STRIP) -s -o $@ $<

targets := vmfreax.elf
$(obj)/vmfreax.elf: vmfreax FORCE
	$(call if_changed,strip)

targets += vmfreax.efi
$(obj)/vmfreax.efi: vmfreax FORCE
	$(call if_changed,objcopy)

EFI_ZBOOT_PAYLOAD      := vmfreax.efi
EFI_ZBOOT_BFD_TARGET   := elf64-loongarch
EFI_ZBOOT_MACH_TYPE    := LOONGARCH64

include $(srctree)/drivers/firmware/efi/libstub/Makefile.zboot
