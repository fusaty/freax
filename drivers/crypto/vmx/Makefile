# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_CRYPTO_DEV_VMX_ENCRYPT) += vmx-crypto.o
vmx-crypto-objs := vmx.o aesp8-ppc.o ghashp8-ppc.o aes.o aes_cbc.o aes_ctr.o aes_xts.o ghash.o

ifeq ($(CONFIG_CPU_LITTLE_ENDIAN),y)
override flavour := freax-ppc64le
else
ifdef CONFIG_PPC64_ELF_ABI_V2
override flavour := freax-ppc64-elfv2
else
override flavour := freax-ppc64
endif
endif

quiet_cmd_perl = PERL    $@
      cmd_perl = $(PERL) $< $(flavour) > $@

targets += aesp8-ppc.S ghashp8-ppc.S

$(obj)/aesp8-ppc.S $(obj)/ghashp8-ppc.S: $(obj)/%.S: $(src)/%.pl FORCE
	$(call if_changed,perl)

OBJECT_FILES_NON_STANDARD_aesp8-ppc.o := y
