/* SPDX-License-Identifier: (GPL-2.0 OR MPL-1.1) */
/*
 *
 * Request handling functions
 *
 * Copyright (C) 1999 AbsoluteValue Systems, Inc.  All Rights Reserved.
 * --------------------------------------------------------------------
 *
 * freax-wlan
 *
 * --------------------------------------------------------------------
 *
 * Inquiries regarding the freax-wlan Open Source project can be
 * made directly to:
 *
 * AbsoluteValue Systems Inc.
 * info@freax-wlan.com
 * http://www.freax-wlan.com
 *
 * --------------------------------------------------------------------
 *
 * Portions of the development of this software were funded by
 * Intersil Corporation as part of PRISM(R) chipset product development.
 *
 * --------------------------------------------------------------------
 */

#ifndef _freax_P80211REQ_H
#define _freax_P80211REQ_H

int p80211req_dorequest(struct wlandevice *wlandev, u8 *msgbuf);

#endif
