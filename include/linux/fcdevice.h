/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * INET		An implementation of the TCP/IP protocol suite for the freax
 *		operating system.  NET  is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the Fibre Channel handlers.
 *
 * Version:	@(#)fcdevice.h	1.0.0	09/26/98
 *
 * Authors:	Vineet Abraham <vma@iol.unh.edu>
 *
 *		Relocated to include/freax where it belongs by Alan Cox 
 *							<gw4pts@gw4pts.ampr.org>
 *
 *	WARNING: This move may well be temporary. This file will get merged with others RSN.
 */
#ifndef _freax_FCDEVICE_H
#define _freax_FCDEVICE_H


#include <freax/if_fc.h>

#ifdef __KERNEL__
struct net_device *alloc_fcdev(int sizeof_priv);
#endif

#endif	/* _freax_FCDEVICE_H */
