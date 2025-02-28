/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef _RTW_XMIT_H_
#define _RTW_XMIT_H_

#include <freax/completion.h>

#define MAX_XMITBUF_SZ	(20480)	/*  20k */

#define NR_XMITBUFF	(16)

#define XMITBUF_ALIGN_SZ 512

/*  xmit extension buff defination */
#define MAX_XMIT_EXTBUF_SZ	(1536)
#define NR_XMIT_EXTBUFF	(32)

#define MAX_CMDBUF_SZ	(5120)	/* 4096) */

#define MAX_NUMBLKS		(1)

#define XMIT_VO_QUEUE (0)
#define XMIT_VI_QUEUE (1)
#define XMIT_BE_QUEUE (2)
#define XMIT_BK_QUEUE (3)

#define VO_QUEUE_INX		0
#define VI_QUEUE_INX		1
#define BE_QUEUE_INX		2
#define BK_QUEUE_INX		3
#define BCN_QUEUE_INX		4
#define MGT_QUEUE_INX		5
#define HIGH_QUEUE_INX		6
#define TXCMD_QUEUE_INX	7

#define HW_QUEUE_ENTRY	8

#define WEP_IV(pattrib_iv, dot11txpn, keyidx)\
do {\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = dot11txpn._byte_.TSC2;\
	pattrib_iv[3] = ((keyidx & 0x3)<<6);\
	dot11txpn.val = (dot11txpn.val == 0xffffff) ? 0 : (dot11txpn.val + 1);\
} while (0)


#define TKIP_IV(pattrib_iv, dot11txpn, keyidx)\
do {\
	pattrib_iv[0] = dot11txpn._byte_.TSC1;\
	pattrib_iv[1] = (dot11txpn._byte_.TSC1 | 0x20) & 0x7f;\
	pattrib_iv[2] = dot11txpn._byte_.TSC0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0 : (dot11txpn.val + 1);\
} while (0)

#define AES_IV(pattrib_iv, dot11txpn, keyidx)\
do {\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = 0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0 : (dot11txpn.val + 1);\
} while (0)


#define HWXMIT_ENTRY	4

/*  For Buffer Descriptor ring architecture */
#define TXDESC_SIZE 40

#define TXDESC_OFFSET TXDESC_SIZE

#define TXDESC_40_BYTES

struct tx_desc {
	__le32 txdw0;
	__le32 txdw1;
	__le32 txdw2;
	__le32 txdw3;
	__le32 txdw4;
	__le32 txdw5;
	__le32 txdw6;
	__le32 txdw7;

#if defined(TXDESC_40_BYTES) || defined(TXDESC_64_BYTES)
	__le32 txdw8;
	__le32 txdw9;
#endif /*  TXDESC_40_BYTES */

#ifdef TXDESC_64_BYTES
	__le32 txdw10;
	__le32 txdw11;

	/*  2008/05/15 MH Because PCIE HW memory R/W 4K limit. And now,  our descriptor */
	/*  size is 40 bytes. If you use more than 102 descriptor(103*40>4096), HW will execute */
	/*  memoryR/W CRC error. And then all DMA fetch will fail. We must decrease descriptor */
	/*  number or enlarge descriptor size as 64 bytes. */
	__le32 txdw12;
	__le32 txdw13;
	__le32 txdw14;
	__le32 txdw15;
#endif
};

union txdesc {
	struct tx_desc txdesc;
	unsigned int value[TXDESC_SIZE>>2];
};

struct	hw_xmit	{
	/* spinlock_t xmit_lock; */
	/* struct list_head	pending; */
	struct __queue *sta_queue;
	/* struct hw_txqueue *phwtxqueue; */
	/* signed int	txcmdcnt; */
	int	accnt;
};

/* reduce size */
struct pkt_attrib {
	u8 type;
	u8 subtype;
	u8 bswenc;
	u8 dhcp_pkt;
	u16 ether_type;
	u16 seqnum;
	u16 pkt_hdrlen;	/* the original 802.3 pkt header len */
	u16 hdrlen;		/* the WLAN Header Len */
	u32 pktlen;		/* the original 802.3 pkt raw_data len (not include ether_hdr data) */
	u32 last_txcmdsz;
	u8 nr_frags;
	u8 encrypt;	/* when 0 indicates no encryption; when non-zero, indicates the encryption algorithm */
	u8 iv_len;
	u8 icv_len;
	u8 iv[18];
	u8 icv[16];
	u8 priority;
	u8 ack_policy;
	u8 mac_id;
	u8 vcs_mode;	/* virtual carrier sense method */
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 key_idx;
	u8 qos_en;
	u8 ht_en;
	u8 raid;/* rate adpative id */
	u8 bwmode;
	u8 ch_offset;/* PRIME_CHNL_OFFSET */
	u8 sgi;/* short GI */
	u8 ampdu_en;/* tx ampdu enable */
	u8 ampdu_spacing; /* ampdu_min_spacing for peer sta's rx */
	u8 mdata;/* more data bit */
	u8 pctrl;/* per packet txdesc control enable */
	u8 triggered;/* for ap mode handling Power Saving sta */
	u8 qsel;
	u8 order;/* order bit */
	u8 eosp;
	u8 rate;
	u8 intel_proxim;
	u8 retry_ctrl;
	u8   mbssid;
	u8 ldpc;
	u8 stbc;
	struct sta_info *psta;

	u8 rtsen;
	u8 cts2self;
	union Keytype	dot11tkiptxmickey;
	/* union Keytype	dot11tkiprxmickey; */
	union Keytype	dot118021x_UncstKey;

	u8 icmp_pkt;

};

#define WLANHDR_OFFSET	64

#define NULL_FRAMETAG		(0x0)
#define DATA_FRAMETAG		0x01
#define L2_FRAMETAG		0x02
#define MGNT_FRAMETAG		0x03
#define AMSDU_FRAMETAG	0x04

#define EII_FRAMETAG		0x05
#define IEEE8023_FRAMETAG  0x06

#define MP_FRAMETAG		0x07

#define TXAGG_FRAMETAG	0x08

enum {
	XMITBUF_DATA = 0,
	XMITBUF_MGNT = 1,
	XMITBUF_CMD = 2,
};

struct  submit_ctx {
	unsigned long submit_time; /* */
	u32 timeout_ms; /* <0: not synchronous, 0: wait forever, >0: up to ms waiting */
	int status; /* status for operation */
	struct completion done;
};

enum {
	RTW_SCTX_SUBMITTED = -1,
	RTW_SCTX_DONE_SUCCESS = 0,
	RTW_SCTX_DONE_UNKNOWN,
	RTW_SCTX_DONE_TIMEOUT,
	RTW_SCTX_DONE_BUF_ALLOC,
	RTW_SCTX_DONE_BUF_FREE,
	RTW_SCTX_DONE_WRITE_PORT_ERR,
	RTW_SCTX_DONE_TX_DESC_NA,
	RTW_SCTX_DONE_TX_DENY,
	RTW_SCTX_DONE_CCX_PKT_FAIL,
	RTW_SCTX_DONE_DRV_STOP,
	RTW_SCTX_DONE_DEV_REMOVE,
	RTW_SCTX_DONE_CMD_ERROR,
};


void rtw_sctx_init(struct submit_ctx *sctx, int timeout_ms);
int rtw_sctx_wait(struct submit_ctx *sctx);
void rtw_sctx_done_err(struct submit_ctx **sctx, int status);
void rtw_sctx_done(struct submit_ctx **sctx);

struct xmit_buf {
	struct list_head	list;

	struct adapter *padapter;

	u8 *pallocated_buf;

	u8 *pbuf;

	void *priv_data;

	u16 buf_tag; /*  0: Normal xmitbuf, 1: extension xmitbuf, 2:cmd xmitbuf */
	u16 flags;
	u32 alloc_sz;

	u32  len;

	struct submit_ctx *sctx;

	u8 *phead;
	u8 *pdata;
	u8 *ptail;
	u8 *pend;
	u32 ff_hwaddr;
	u8 pg_num;
	u8 agg_num;

#if defined(DBG_XMIT_BUF) || defined(DBG_XMIT_BUF_EXT)
	u8 no;
#endif

};


struct xmit_frame {
	struct list_head	list;

	struct pkt_attrib attrib;

	struct sk_buff *pkt;

	int	frame_tag;

	struct adapter *padapter;

	u8 *buf_addr;

	struct xmit_buf *pxmitbuf;

	u8 pg_num;
	u8 agg_num;

	u8 ack_report;

	u8 *alloc_addr; /* the actual address this xmitframe allocated */
	u8 ext_tag; /* 0:data, 1:mgmt */

};

struct tx_servq {
	struct list_head	tx_pending;
	struct __queue	sta_pending;
	int qcnt;
};


struct sta_xmit_priv {
	spinlock_t	lock;
	signed int	option;
	signed int	apsd_setting;	/* When bit mask is on, the associated edca queue supports APSD. */


	/* struct tx_servq blk_q[MAX_NUMBLKS]; */
	struct tx_servq	be_q;			/* priority == 0, 3 */
	struct tx_servq	bk_q;			/* priority == 1, 2 */
	struct tx_servq	vi_q;			/* priority == 4, 5 */
	struct tx_servq	vo_q;			/* priority == 6, 7 */
	struct list_head	legacy_dz;
	struct list_head  apsd;

	u16 txseq_tid[16];

	/* uint	sta_tx_bytes; */
	/* u64	sta_tx_pkts; */
	/* uint	sta_tx_fail; */


};


struct	hw_txqueue	{
	volatile signed int	head;
	volatile signed int	tail;
	volatile signed int	free_sz;	/* in units of 64 bytes */
	volatile signed int      free_cmdsz;
	volatile signed int	 txsz[8];
	uint	ff_hwaddr;
	uint	cmd_hwaddr;
	signed int	ac_tag;
};

enum cmdbuf_type {
	CMDBUF_BEACON = 0x00,
	CMDBUF_RSVD,
	CMDBUF_MAX
};

struct	xmit_priv {

	spinlock_t	lock;

	struct completion xmit_comp;
	struct completion terminate_xmitthread_comp;

	/* struct __queue	blk_strms[MAX_NUMBLKS]; */
	struct __queue	be_pending;
	struct __queue	bk_pending;
	struct __queue	vi_pending;
	struct __queue	vo_pending;
	struct __queue	bm_pending;

	/* struct __queue	legacy_dz_queue; */
	/* struct __queue	apsd_queue; */

	u8 *pallocated_frame_buf;
	u8 *pxmit_frame_buf;
	uint free_xmitframe_cnt;
	struct __queue	free_xmit_queue;

	/* uint mapping_addr; */
	/* uint pkt_sz; */

	u8 *xframe_ext_alloc_addr;
	u8 *xframe_ext;
	uint free_xframe_ext_cnt;
	struct __queue free_xframe_ext_queue;

	/* struct	hw_txqueue	be_txqueue; */
	/* struct	hw_txqueue	bk_txqueue; */
	/* struct	hw_txqueue	vi_txqueue; */
	/* struct	hw_txqueue	vo_txqueue; */
	/* struct	hw_txqueue	bmc_txqueue; */

	uint	frag_len;

	struct adapter	*adapter;

	u8   vcs_setting;
	u8 vcs;
	u8 vcs_type;
	/* u16  rts_thresh; */

	u64	tx_bytes;
	u64	tx_pkts;
	u64	tx_drop;
	u64	last_tx_pkts;

	struct hw_xmit *hwxmits;
	u8 hwxmit_entry;

	u8 wmm_para_seq[4];/* sequence for wmm ac parameter strength from large to small. it's value is 0->vo, 1->vi, 2->be, 3->bk. */

	void *SdioXmitThread;
	struct completion SdioXmitStart;
	struct completion SdioXmitTerminate;

	struct __queue free_xmitbuf_queue;
	struct __queue pending_xmitbuf_queue;
	u8 *pallocated_xmitbuf;
	u8 *pxmitbuf;
	uint free_xmitbuf_cnt;

	struct __queue free_xmit_extbuf_queue;
	u8 *pallocated_xmit_extbuf;
	u8 *pxmit_extbuf;
	uint free_xmit_extbuf_cnt;

	struct xmit_buf	pcmd_xmitbuf[CMDBUF_MAX];

	u16 nqos_ssn;

	int	ack_tx;
	struct mutex ack_tx_mutex;
	struct submit_ctx ack_tx_ops;
	u8 seq_no;
	spinlock_t lock_sctx;
};

extern struct xmit_frame *__rtw_alloc_cmdxmitframe(struct xmit_priv *pxmitpriv,
		enum cmdbuf_type buf_type);
#define rtw_alloc_cmdxmitframe(p) __rtw_alloc_cmdxmitframe(p, CMDBUF_RSVD)
#define rtw_alloc_bcnxmitframe(p) __rtw_alloc_cmdxmitframe(p, CMDBUF_BEACON)

extern struct xmit_buf *rtw_alloc_xmitbuf_ext(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitbuf_ext(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);

extern struct xmit_buf *rtw_alloc_xmitbuf(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);

void rtw_count_tx_stats(struct adapter *padapter, struct xmit_frame *pxmitframe, int sz);
extern void rtw_update_protection(struct adapter *padapter, u8 *ie, uint ie_len);
extern s32 rtw_make_wlanhdr(struct adapter *padapter, u8 *hdr, struct pkt_attrib *pattrib);
extern s32 rtw_put_snap(u8 *data, u16 h_proto);

extern struct xmit_frame *rtw_alloc_xmitframe(struct xmit_priv *pxmitpriv);
struct xmit_frame *rtw_alloc_xmitframe_ext(struct xmit_priv *pxmitpriv);
struct xmit_frame *rtw_alloc_xmitframe_once(struct xmit_priv *pxmitpriv);
extern s32 rtw_free_xmitframe(struct xmit_priv *pxmitpriv, struct xmit_frame *pxmitframe);
extern void rtw_free_xmitframe_queue(struct xmit_priv *pxmitpriv, struct __queue *pframequeue);
struct tx_servq *rtw_get_sta_pending(struct adapter *padapter, struct sta_info *psta, signed int up, u8 *ac);
extern s32 rtw_xmitframe_enqueue(struct adapter *padapter, struct xmit_frame *pxmitframe);

extern s32 rtw_xmit_classifier(struct adapter *padapter, struct xmit_frame *pxmitframe);
extern u32 rtw_calculate_wlan_pkt_size_by_attribue(struct pkt_attrib *pattrib);
#define rtw_wlan_pkt_size(f) rtw_calculate_wlan_pkt_size_by_attribue(&f->attrib)
extern s32 rtw_xmitframe_coalesce(struct adapter *padapter, struct sk_buff *pkt, struct xmit_frame *pxmitframe);
extern s32 rtw_mgmt_xmitframe_coalesce(struct adapter *padapter, struct sk_buff *pkt, struct xmit_frame *pxmitframe);
s32 _rtw_init_hw_txqueue(struct hw_txqueue *phw_txqueue, u8 ac_tag);
void _rtw_init_sta_xmit_priv(struct sta_xmit_priv *psta_xmitpriv);


s32 rtw_txframes_pending(struct adapter *padapter);
void rtw_init_hwxmits(struct hw_xmit *phwxmit, signed int entry);


s32 _rtw_init_xmit_priv(struct xmit_priv *pxmitpriv, struct adapter *padapter);
void _rtw_free_xmit_priv(struct xmit_priv *pxmitpriv);


s32 rtw_alloc_hwxmits(struct adapter *padapter);
void rtw_free_hwxmits(struct adapter *padapter);


s32 rtw_xmit(struct adapter *padapter, struct sk_buff **pkt);
bool xmitframe_hiq_filter(struct xmit_frame *xmitframe);

signed int xmitframe_enqueue_for_sleeping_sta(struct adapter *padapter, struct xmit_frame *pxmitframe);
void stop_sta_xmit(struct adapter *padapter, struct sta_info *psta);
void wakeup_sta_to_xmit(struct adapter *padapter, struct sta_info *psta);
void xmit_delivery_enabled_frames(struct adapter *padapter, struct sta_info *psta);

u8 query_ra_short_GI(struct sta_info *psta);

u8 qos_acm(u8 acm_mask, u8 priority);

void enqueue_pending_xmitbuf(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);
void enqueue_pending_xmitbuf_to_head(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf);
struct xmit_buf *dequeue_pending_xmitbuf(struct xmit_priv *pxmitpriv);
struct xmit_buf *dequeue_pending_xmitbuf_under_survey(struct xmit_priv *pxmitpriv);
signed int	check_pending_xmitbuf(struct xmit_priv *pxmitpriv);
int	rtw_xmit_thread(void *context);

u32 rtw_get_ff_hwaddr(struct xmit_frame	*pxmitframe);

int rtw_ack_tx_wait(struct xmit_priv *pxmitpriv, u32 timeout_ms);
void rtw_ack_tx_done(struct xmit_priv *pxmitpriv, int status);

/* include after declaring struct xmit_buf, in order to avoid warning */
#include <xmit_osdep.h>

#endif	/* _RTL871X_XMIT_H_ */
