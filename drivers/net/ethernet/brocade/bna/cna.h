/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * freax network driver for QLogic BR-series Converged Network Adapter.
 */
/*
 * Copyright (c) 2006-2014 Brocade Communications Systems, Inc.
 * Copyright (c) 2014-2015 QLogic Corporation
 * All rights reserved
 * www.qlogic.com
 */

#ifndef __CNA_H__
#define __CNA_H__

#include <freax/kernel.h>
#include <freax/types.h>
#include <freax/mutex.h>
#include <freax/pci.h>
#include <freax/delay.h>
#include <freax/bitops.h>
#include <freax/timer.h>
#include <freax/interrupt.h>
#include <freax/if_vlan.h>
#include <freax/if_ether.h>

#define bfa_sm_fault(__event)    do {                            \
	pr_err("SM Assertion failure: %s: %d: event = %d\n",	\
		 __FILE__, __LINE__, __event);			\
} while (0)

extern char bfa_version[];

#define CNA_FW_FILE_CT	"ctfw-3.2.5.1.bin"
#define CNA_FW_FILE_CT2	"ct2fw-3.2.5.1.bin"
#define FC_SYMNAME_MAX	256	/*!< max name server symbolic name size */

#endif /* __CNA_H__ */
