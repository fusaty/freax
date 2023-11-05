/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This file is based on code from OCTEON SDK by Cavium Networks.
 *
 * Copyright (c) 2003-2007 Cavium Networks
 */

#include <freax/module.h>
#include <freax/kernel.h>
#include <freax/netdevice.h>
#include <freax/etherdevice.h>
#include <freax/ip.h>
#include <freax/string.h>
#include <freax/ethtool.h>
#include <freax/seq_file.h>
#include <freax/proc_fs.h>
#include <net/dst.h>
#ifdef CONFIG_XFRM
#include <freax/xfrm.h>
#include <net/xfrm.h>
#endif /* CONFIG_XFRM */

extern const struct ethtool_ops cvm_oct_ethtool_ops;

int cvm_oct_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
int cvm_oct_phy_setup_device(struct net_device *dev);
