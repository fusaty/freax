# SPDX-License-Identifier: GPL-2.0
#
# create a compressed vmfreax image from the original vmfreax
#

targets		:= vmfreax head.o misc.o piggy.o vmfreax.lds
asflags-y	:=

OBJECTS = $(obj)/head.o $(obj)/misc.o

LDFLAGS_vmfreax := -T

$(obj)/vmfreax: $(obj)/vmfreax.lds $(OBJECTS) $(obj)/piggy.o FORCE
	$(call if_changed,ld)

LDFLAGS_piggy.o := -r --format binary --oformat elf32-littlenios2 -T

$(obj)/piggy.o: $(obj)/vmfreax.scr $(obj)/../vmfreax.gz FORCE
	$(call if_changed,ld)
