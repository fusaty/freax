/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 *
 * Modifications for inclusion into the freax staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/
#ifndef _WIFI_H_
#define _WIFI_H_

#include <freax/compiler.h>
#include <freax/ieee80211.h>

#define WLAN_HDR_A3_LEN		24
#define WLAN_HDR_A3_QOS_LEN	26

enum WIFI_FRAME_TYPE {
	WIFI_QOS_DATA_TYPE	= (BIT(7) | BIT(3)),	/*!< QoS Data */
};

#define SetToDs(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_TODS); \
})

#define GetToDs(pbuf)	(((*(__le16 *)(pbuf)) & cpu_to_le16(IEEE80211_FCTL_TODS)) != 0)

#define ClearToDs(pbuf)	({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_TODS)); \
})

#define SetFrDs(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_FROMDS); \
})

#define GetFrDs(pbuf)	(((*(__le16 *)(pbuf)) & cpu_to_le16(IEEE80211_FCTL_FROMDS)) != 0)

#define ClearFrDs(pbuf)	({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_FROMDS)); \
})

static inline unsigned char get_tofr_ds(unsigned char *pframe)
{
	return ((GetToDs(pframe) << 1) | GetFrDs(pframe));
}

#define SetMFrag(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_MOREFRAGS); \
})

#define GetMFrag(pbuf)	(((*(__le16 *)(pbuf)) & cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)) != 0)

#define ClearMFrag(pbuf) ({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)); \
})

#define SetRetry(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_RETRY); \
})

#define GetRetry(pbuf)	(((*(__le16 *)(pbuf)) & cpu_to_le16(IEEE80211_FCTL_RETRY)) != 0)

#define ClearRetry(pbuf) ({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_RETRY)); \
})

#define SetPwrMgt(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_PM); \
})

#define GetPwrMgt(pbuf)	(((*(__le16 *)(pbuf)) & \
			cpu_to_le16(IEEE80211_FCTL_PM)) != 0)

#define ClearPwrMgt(pbuf) ({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_PM)); \
})

#define SetMData(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_MOREDATA); \
})

#define GetMData(pbuf)	(((*(__le16 *)(pbuf)) & \
			cpu_to_le16(IEEE80211_FCTL_MOREDATA)) != 0)

#define ClearMData(pbuf) ({ \
	*(__le16 *)(pbuf) &= (~cpu_to_le16(IEEE80211_FCTL_MOREDATA)); \
})

#define SetPrivacy(pbuf) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(IEEE80211_FCTL_PROTECTED); \
})

#define GetPrivacy(pbuf)	(((*(__le16 *)(pbuf)) & \
				cpu_to_le16(IEEE80211_FCTL_PROTECTED)) != 0)

#define GetOrder(pbuf)	(((*(__le16 *)(pbuf)) & \
			cpu_to_le16(IEEE80211_FCTL_ORDER)) != 0)

#define GetFrameType(pbuf)	(le16_to_cpu(*(__le16 *)(pbuf)) & \
				(BIT(3) | BIT(2)))

#define SetFrameType(pbuf, type)	\
	do {	\
		*(__le16 *)(pbuf) &= cpu_to_le16(~(BIT(3) | \
		BIT(2))); \
		*(__le16 *)(pbuf) |= cpu_to_le16(type); \
	} while (0)

#define GetFrameSubType(pbuf)	(le16_to_cpu(*(__le16 *)(pbuf)) & \
				(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | \
				BIT(2)))

#define SetFrameSubType(pbuf, type) \
	do {    \
		*(__le16 *)(pbuf) &= cpu_to_le16(~(BIT(7) | BIT(6) | \
		BIT(5) | BIT(4) | BIT(3) | BIT(2))); \
		*(__le16 *)(pbuf) |= cpu_to_le16(type); \
	} while (0)

#define GetSequence(pbuf)	(le16_to_cpu(*(__le16 *)\
				((addr_t)(pbuf) + 22)) >> 4)

#define GetFragNum(pbuf)	(le16_to_cpu(*(__le16 *)((addr_t)\
				(pbuf) + 22)) & 0x0f)

#define SetSeqNum(pbuf, num) ({ \
	*(__le16 *)((addr_t)(pbuf) + 22) = \
	cpu_to_le16((le16_to_cpu(*(__le16 *)((addr_t)(pbuf) + 22)) & \
	0x000f) | (0xfff0 & (num << 4))); \
})

#define SetPriority(pbuf, tid) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16(tid & 0xf); \
})

#define GetPriority(pbuf)	((le16_to_cpu(*(__le16 *)(pbuf))) & 0xf)

#define SetAckpolicy(pbuf, ack) ({ \
	*(__le16 *)(pbuf) |= cpu_to_le16((ack & 3) << 5); \
})

#define GetAckpolicy(pbuf) (((le16_to_cpu(*(__le16 *)pbuf)) >> 5) & 0x3)

#define GetAMsdu(pbuf) (((le16_to_cpu(*(__le16 *)pbuf)) >> 7) & 0x1)

#define GetAddr1Ptr(pbuf)	((unsigned char *)((addr_t)(pbuf) + 4))

#define GetAddr2Ptr(pbuf)	((unsigned char *)((addr_t)(pbuf) + 10))

#define GetAddr3Ptr(pbuf)	((unsigned char *)((addr_t)(pbuf) + 16))

#define GetAddr4Ptr(pbuf)	((unsigned char *)((addr_t)(pbuf) + 24))

static inline unsigned char *get_hdr_bssid(unsigned char *pframe)
{
	unsigned char	*sa;
	unsigned int	to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

	switch (to_fr_ds) {
	case 0x00:	/* ToDs=0, FromDs=0 */
		sa = GetAddr3Ptr(pframe);
		break;
	case 0x01:	/* ToDs=0, FromDs=1 */
		sa = GetAddr2Ptr(pframe);
		break;
	case 0x02:	/* ToDs=1, FromDs=0 */
		sa = GetAddr1Ptr(pframe);
		break;
	default:	/* ToDs=1, FromDs=1 */
		sa = NULL;
		break;
	}
	return sa;
}

/* ---------------------------------------------------------------------------
 *			Below is the fixed elements...
 * ---------------------------------------------------------------------------
 */
#define _BEACON_ITERVAL_		2
#define _CAPABILITY_			2
#define _TIMESTAMP_				8

/*-----------------------------------------------------------------------------
 *			Below is the definition for WMM
 *------------------------------------------------------------------------------
 */
#define _WMM_IE_Length_				7  /* for WMM STA */

#endif /* _WIFI_H_ */

