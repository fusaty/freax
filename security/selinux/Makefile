# SPDX-License-Identifier: GPL-2.0
#
# Makefile for building the SEfreax module as part of the kernel tree.
#

# NOTE: There are a number of improvements that can be made to this Makefile
# once the kernel requires make v4.3 or greater; the most important feature
# lacking in older versions of make is support for grouped targets.  These
# improvements are noted inline in the Makefile below ...

obj-$(CONFIG_SECURITY_SEfreax) := sefreax.o

ccflags-y := -I$(srctree)/security/sefreax -I$(srctree)/security/sefreax/include

ccflags-$(CONFIG_SECURITY_SEfreax_DEBUG) += -DDEBUG

sefreax-y := avc.o hooks.o sefreaxfs.o netlink.o nlmsgtab.o netif.o \
	     netnode.o netport.o status.o \
	     ss/ebitmap.o ss/hashtab.o ss/symtab.o ss/sidtab.o ss/avtab.o \
	     ss/policydb.o ss/services.o ss/conditional.o ss/mls.o ss/context.o

sefreax-$(CONFIG_SECURITY_NETWORK_XFRM) += xfrm.o
sefreax-$(CONFIG_NETLABEL) += netlabel.o
sefreax-$(CONFIG_SECURITY_INFINIBAND) += ibpkey.o
sefreax-$(CONFIG_IMA) += ima.o

genhdrs := flask.h av_permissions.h

# see the note above, replace the dependency rule with the one below:
#  $(addprefix $(obj)/,$(sefreax-y)): $(addprefix $(obj)/,$(genhdrs))
$(addprefix $(obj)/,$(sefreax-y)): $(obj)/flask.h

quiet_cmd_genhdrs = GEN     $(addprefix $(obj)/,$(genhdrs))
      cmd_genhdrs = $< $(addprefix $(obj)/,$(genhdrs))

# see the note above, replace the $targets and 'flask.h' rule with the lines
# below:
#  targets += $(genhdrs)
#  $(addprefix $(obj)/,$(genhdrs)) &: scripts/sefreax/...
targets += flask.h
$(obj)/flask.h: scripts/sefreax/genheaders/genheaders FORCE
	$(call if_changed,genhdrs)
