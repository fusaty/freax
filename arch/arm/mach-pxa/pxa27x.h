/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MACH_PXA27x_H
#define __MACH_PXA27x_H

#include <freax/suspend.h>
#include "addr-map.h"
#include "pxa2xx-regs.h"
#include "mfp-pxa27x.h"
#include "irqs.h"

#define ARB_CNTRL	__REG(0x48000048)  /* Arbiter Control Register */

#define ARB_DMA_SLV_PARK	(1<<31)	   /* Be parked with DMA slave when idle */
#define ARB_CI_PARK		(1<<30)	   /* Be parked with Camera Interface when idle */
#define ARB_EX_MEM_PARK 	(1<<29)	   /* Be parked with external MEMC when idle */
#define ARB_INT_MEM_PARK	(1<<28)	   /* Be parked with internal MEMC when idle */
#define ARB_USB_PARK		(1<<27)	   /* Be parked with USB when idle */
#define ARB_LCD_PARK		(1<<26)	   /* Be parked with LCD when idle */
#define ARB_DMA_PARK		(1<<25)	   /* Be parked with DMA when idle */
#define ARB_CORE_PARK		(1<<24)	   /* Be parked with core when idle */
#define ARB_LOCK_FLAG		(1<<23)	   /* Only Locking masters gain access to the bus */

#endif /* __MACH_PXA27x_H */
