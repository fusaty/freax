// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2005-2011 Atheros Communications Inc.
 * Copyright (c) 2011-2017 Qualcomm Atheros, Inc.
 */

#include <freax/slab.h>
#include <freax/if_ether.h>

#include "htt.h"
#include "core.h"
#include "debug.h"
#include "hif.h"

static const enum htt_t2h_msg_type htt_main_t2h_msg_types[] = {
	[HTT_MAIN_T2H_MSG_TYPE_VERSION_CONF] = HTT_T2H_MSG_TYPE_VERSION_CONF,
	[HTT_MAIN_T2H_MSG_TYPE_RX_IND] = HTT_T2H_MSG_TYPE_RX_IND,
	[HTT_MAIN_T2H_MSG_TYPE_RX_FLUSH] = HTT_T2H_MSG_TYPE_RX_FLUSH,
	[HTT_MAIN_T2H_MSG_TYPE_PEER_MAP] = HTT_T2H_MSG_TYPE_PEER_MAP,
	[HTT_MAIN_T2H_MSG_TYPE_PEER_UNMAP] = HTT_T2H_MSG_TYPE_PEER_UNMAP,
	[HTT_MAIN_T2H_MSG_TYPE_RX_ADDBA] = HTT_T2H_MSG_TYPE_RX_ADDBA,
	[HTT_MAIN_T2H_MSG_TYPE_RX_DELBA] = HTT_T2H_MSG_TYPE_RX_DELBA,
	[HTT_MAIN_T2H_MSG_TYPE_TX_COMPL_IND] = HTT_T2H_MSG_TYPE_TX_COMPL_IND,
	[HTT_MAIN_T2H_MSG_TYPE_PKTLOG] = HTT_T2H_MSG_TYPE_PKTLOG,
	[HTT_MAIN_T2H_MSG_TYPE_STATS_CONF] = HTT_T2H_MSG_TYPE_STATS_CONF,
	[HTT_MAIN_T2H_MSG_TYPE_RX_FRAG_IND] = HTT_T2H_MSG_TYPE_RX_FRAG_IND,
	[HTT_MAIN_T2H_MSG_TYPE_SEC_IND] = HTT_T2H_MSG_TYPE_SEC_IND,
	[HTT_MAIN_T2H_MSG_TYPE_TX_INSPECT_IND] =
		HTT_T2H_MSG_TYPE_TX_INSPECT_IND,
	[HTT_MAIN_T2H_MSG_TYPE_MGMT_TX_COMPL_IND] =
		HTT_T2H_MSG_TYPE_MGMT_TX_COMPLETION,
	[HTT_MAIN_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND] =
		HTT_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND,
	[HTT_MAIN_T2H_MSG_TYPE_RX_PN_IND] = HTT_T2H_MSG_TYPE_RX_PN_IND,
	[HTT_MAIN_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND] =
		HTT_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND,
	[HTT_MAIN_T2H_MSG_TYPE_TEST] = HTT_T2H_MSG_TYPE_TEST,
};

static const enum htt_t2h_msg_type htt_10x_t2h_msg_types[] = {
	[HTT_10X_T2H_MSG_TYPE_VERSION_CONF] = HTT_T2H_MSG_TYPE_VERSION_CONF,
	[HTT_10X_T2H_MSG_TYPE_RX_IND] = HTT_T2H_MSG_TYPE_RX_IND,
	[HTT_10X_T2H_MSG_TYPE_RX_FLUSH] = HTT_T2H_MSG_TYPE_RX_FLUSH,
	[HTT_10X_T2H_MSG_TYPE_PEER_MAP] = HTT_T2H_MSG_TYPE_PEER_MAP,
	[HTT_10X_T2H_MSG_TYPE_PEER_UNMAP] = HTT_T2H_MSG_TYPE_PEER_UNMAP,
	[HTT_10X_T2H_MSG_TYPE_RX_ADDBA] = HTT_T2H_MSG_TYPE_RX_ADDBA,
	[HTT_10X_T2H_MSG_TYPE_RX_DELBA] = HTT_T2H_MSG_TYPE_RX_DELBA,
	[HTT_10X_T2H_MSG_TYPE_TX_COMPL_IND] = HTT_T2H_MSG_TYPE_TX_COMPL_IND,
	[HTT_10X_T2H_MSG_TYPE_PKTLOG] = HTT_T2H_MSG_TYPE_PKTLOG,
	[HTT_10X_T2H_MSG_TYPE_STATS_CONF] = HTT_T2H_MSG_TYPE_STATS_CONF,
	[HTT_10X_T2H_MSG_TYPE_RX_FRAG_IND] = HTT_T2H_MSG_TYPE_RX_FRAG_IND,
	[HTT_10X_T2H_MSG_TYPE_SEC_IND] = HTT_T2H_MSG_TYPE_SEC_IND,
	[HTT_10X_T2H_MSG_TYPE_RC_UPDATE_IND] = HTT_T2H_MSG_TYPE_RC_UPDATE_IND,
	[HTT_10X_T2H_MSG_TYPE_TX_INSPECT_IND] = HTT_T2H_MSG_TYPE_TX_INSPECT_IND,
	[HTT_10X_T2H_MSG_TYPE_TEST] = HTT_T2H_MSG_TYPE_TEST,
	[HTT_10X_T2H_MSG_TYPE_CHAN_CHANGE] = HTT_T2H_MSG_TYPE_CHAN_CHANGE,
	[HTT_10X_T2H_MSG_TYPE_AGGR_CONF] = HTT_T2H_MSG_TYPE_AGGR_CONF,
	[HTT_10X_T2H_MSG_TYPE_STATS_NOUPLOAD] = HTT_T2H_MSG_TYPE_STATS_NOUPLOAD,
	[HTT_10X_T2H_MSG_TYPE_MGMT_TX_COMPL_IND] =
		HTT_T2H_MSG_TYPE_MGMT_TX_COMPLETION,
};

static const enum htt_t2h_msg_type htt_tlv_t2h_msg_types[] = {
	[HTT_TLV_T2H_MSG_TYPE_VERSION_CONF] = HTT_T2H_MSG_TYPE_VERSION_CONF,
	[HTT_TLV_T2H_MSG_TYPE_RX_IND] = HTT_T2H_MSG_TYPE_RX_IND,
	[HTT_TLV_T2H_MSG_TYPE_RX_FLUSH] = HTT_T2H_MSG_TYPE_RX_FLUSH,
	[HTT_TLV_T2H_MSG_TYPE_PEER_MAP] = HTT_T2H_MSG_TYPE_PEER_MAP,
	[HTT_TLV_T2H_MSG_TYPE_PEER_UNMAP] = HTT_T2H_MSG_TYPE_PEER_UNMAP,
	[HTT_TLV_T2H_MSG_TYPE_RX_ADDBA] = HTT_T2H_MSG_TYPE_RX_ADDBA,
	[HTT_TLV_T2H_MSG_TYPE_RX_DELBA] = HTT_T2H_MSG_TYPE_RX_DELBA,
	[HTT_TLV_T2H_MSG_TYPE_TX_COMPL_IND] = HTT_T2H_MSG_TYPE_TX_COMPL_IND,
	[HTT_TLV_T2H_MSG_TYPE_PKTLOG] = HTT_T2H_MSG_TYPE_PKTLOG,
	[HTT_TLV_T2H_MSG_TYPE_STATS_CONF] = HTT_T2H_MSG_TYPE_STATS_CONF,
	[HTT_TLV_T2H_MSG_TYPE_RX_FRAG_IND] = HTT_T2H_MSG_TYPE_RX_FRAG_IND,
	[HTT_TLV_T2H_MSG_TYPE_SEC_IND] = HTT_T2H_MSG_TYPE_SEC_IND,
	[HTT_TLV_T2H_MSG_TYPE_RC_UPDATE_IND] = HTT_T2H_MSG_TYPE_RC_UPDATE_IND,
	[HTT_TLV_T2H_MSG_TYPE_TX_INSPECT_IND] = HTT_T2H_MSG_TYPE_TX_INSPECT_IND,
	[HTT_TLV_T2H_MSG_TYPE_MGMT_TX_COMPL_IND] =
		HTT_T2H_MSG_TYPE_MGMT_TX_COMPLETION,
	[HTT_TLV_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND] =
		HTT_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND,
	[HTT_TLV_T2H_MSG_TYPE_RX_PN_IND] = HTT_T2H_MSG_TYPE_RX_PN_IND,
	[HTT_TLV_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND] =
		HTT_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND,
	[HTT_TLV_T2H_MSG_TYPE_RX_IN_ORD_PADDR_IND] =
		HTT_T2H_MSG_TYPE_RX_IN_ORD_PADDR_IND,
	[HTT_TLV_T2H_MSG_TYPE_WDI_IPA_OP_RESPONSE] =
		HTT_T2H_MSG_TYPE_WDI_IPA_OP_RESPONSE,
	[HTT_TLV_T2H_MSG_TYPE_CHAN_CHANGE] = HTT_T2H_MSG_TYPE_CHAN_CHANGE,
	[HTT_TLV_T2H_MSG_TYPE_RX_OFLD_PKT_ERR] =
		HTT_T2H_MSG_TYPE_RX_OFLD_PKT_ERR,
	[HTT_TLV_T2H_MSG_TYPE_TEST] = HTT_T2H_MSG_TYPE_TEST,
};

static const enum htt_t2h_msg_type htt_10_4_t2h_msg_types[] = {
	[HTT_10_4_T2H_MSG_TYPE_VERSION_CONF] = HTT_T2H_MSG_TYPE_VERSION_CONF,
	[HTT_10_4_T2H_MSG_TYPE_RX_IND] = HTT_T2H_MSG_TYPE_RX_IND,
	[HTT_10_4_T2H_MSG_TYPE_RX_FLUSH] = HTT_T2H_MSG_TYPE_RX_FLUSH,
	[HTT_10_4_T2H_MSG_TYPE_PEER_MAP] = HTT_T2H_MSG_TYPE_PEER_MAP,
	[HTT_10_4_T2H_MSG_TYPE_PEER_UNMAP] = HTT_T2H_MSG_TYPE_PEER_UNMAP,
	[HTT_10_4_T2H_MSG_TYPE_RX_ADDBA] = HTT_T2H_MSG_TYPE_RX_ADDBA,
	[HTT_10_4_T2H_MSG_TYPE_RX_DELBA] = HTT_T2H_MSG_TYPE_RX_DELBA,
	[HTT_10_4_T2H_MSG_TYPE_TX_COMPL_IND] = HTT_T2H_MSG_TYPE_TX_COMPL_IND,
	[HTT_10_4_T2H_MSG_TYPE_PKTLOG] = HTT_T2H_MSG_TYPE_PKTLOG,
	[HTT_10_4_T2H_MSG_TYPE_STATS_CONF] = HTT_T2H_MSG_TYPE_STATS_CONF,
	[HTT_10_4_T2H_MSG_TYPE_RX_FRAG_IND] = HTT_T2H_MSG_TYPE_RX_FRAG_IND,
	[HTT_10_4_T2H_MSG_TYPE_SEC_IND] = HTT_T2H_MSG_TYPE_SEC_IND,
	[HTT_10_4_T2H_MSG_TYPE_RC_UPDATE_IND] = HTT_T2H_MSG_TYPE_RC_UPDATE_IND,
	[HTT_10_4_T2H_MSG_TYPE_TX_INSPECT_IND] =
				HTT_T2H_MSG_TYPE_TX_INSPECT_IND,
	[HTT_10_4_T2H_MSG_TYPE_MGMT_TX_COMPL_IND] =
				HTT_T2H_MSG_TYPE_MGMT_TX_COMPLETION,
	[HTT_10_4_T2H_MSG_TYPE_CHAN_CHANGE] = HTT_T2H_MSG_TYPE_CHAN_CHANGE,
	[HTT_10_4_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND] =
				HTT_T2H_MSG_TYPE_TX_CREDIT_UPDATE_IND,
	[HTT_10_4_T2H_MSG_TYPE_RX_PN_IND] = HTT_T2H_MSG_TYPE_RX_PN_IND,
	[HTT_10_4_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND] =
				HTT_T2H_MSG_TYPE_RX_OFFLOAD_DELIVER_IND,
	[HTT_10_4_T2H_MSG_TYPE_TEST] = HTT_T2H_MSG_TYPE_TEST,
	[HTT_10_4_T2H_MSG_TYPE_EN_STATS] = HTT_T2H_MSG_TYPE_EN_STATS,
	[HTT_10_4_T2H_MSG_TYPE_AGGR_CONF] = HTT_T2H_MSG_TYPE_AGGR_CONF,
	[HTT_10_4_T2H_MSG_TYPE_TX_FETCH_IND] =
				HTT_T2H_MSG_TYPE_TX_FETCH_IND,
	[HTT_10_4_T2H_MSG_TYPE_TX_FETCH_CONFIRM] =
				HTT_T2H_MSG_TYPE_TX_FETCH_CONFIRM,
	[HTT_10_4_T2H_MSG_TYPE_STATS_NOUPLOAD] =
				HTT_T2H_MSG_TYPE_STATS_NOUPLOAD,
	[HTT_10_4_T2H_MSG_TYPE_TX_MODE_SWITCH_IND] =
				HTT_T2H_MSG_TYPE_TX_MODE_SWITCH_IND,
	[HTT_10_4_T2H_MSG_TYPE_PEER_STATS] =
				HTT_T2H_MSG_TYPE_PEER_STATS,
};

const struct ath10k_htt_rx_desc_ops qca988x_rx_desc_ops = {
	.rx_desc_size = sizeof(struct htt_rx_desc_v1),
	.rx_desc_msdu_payload_offset = offsetof(struct htt_rx_desc_v1, msdu_payload)
};

static int ath10k_qca99x0_rx_desc_get_l3_pad_bytes(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v1 *rx_desc = container_of(rxd,
						      struct htt_rx_desc_v1,
						      base);

	return MS(__le32_to_cpu(rx_desc->msdu_end.qca99x0.info1),
		  RX_MSDU_END_INFO1_L3_HDR_PAD);
}

static bool ath10k_qca99x0_rx_desc_msdu_limit_error(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v1 *rx_desc = container_of(rxd,
						      struct htt_rx_desc_v1,
						      base);

	return !!(rx_desc->msdu_end.common.info0 &
		  __cpu_to_le32(RX_MSDU_END_INFO0_MSDU_LIMIT_ERR));
}

const struct ath10k_htt_rx_desc_ops qca99x0_rx_desc_ops = {
	.rx_desc_size = sizeof(struct htt_rx_desc_v1),
	.rx_desc_msdu_payload_offset = offsetof(struct htt_rx_desc_v1, msdu_payload),

	.rx_desc_get_l3_pad_bytes = ath10k_qca99x0_rx_desc_get_l3_pad_bytes,
	.rx_desc_get_msdu_limit_error = ath10k_qca99x0_rx_desc_msdu_limit_error,
};

static void ath10k_rx_desc_wcn3990_get_offsets(struct htt_rx_ring_rx_desc_offsets *off)
{
#define desc_offset(x) (offsetof(struct htt_rx_desc_v2, x) / 4)
	off->mac80211_hdr_offset = __cpu_to_le16(desc_offset(rx_hdr_status));
	off->msdu_payload_offset = __cpu_to_le16(desc_offset(msdu_payload));
	off->ppdu_start_offset = __cpu_to_le16(desc_offset(ppdu_start));
	off->ppdu_end_offset = __cpu_to_le16(desc_offset(ppdu_end));
	off->mpdu_start_offset = __cpu_to_le16(desc_offset(mpdu_start));
	off->mpdu_end_offset = __cpu_to_le16(desc_offset(mpdu_end));
	off->msdu_start_offset = __cpu_to_le16(desc_offset(msdu_start));
	off->msdu_end_offset = __cpu_to_le16(desc_offset(msdu_end));
	off->rx_attention_offset = __cpu_to_le16(desc_offset(attention));
	off->frag_info_offset = __cpu_to_le16(desc_offset(frag_info));
#undef desc_offset
}

static struct htt_rx_desc *
ath10k_rx_desc_wcn3990_from_raw_buffer(void *buff)
{
	return &((struct htt_rx_desc_v2 *)buff)->base;
}

static struct rx_attention *
ath10k_rx_desc_wcn3990_get_attention(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->attention;
}

static struct rx_frag_info_common *
ath10k_rx_desc_wcn3990_get_frag_info(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->frag_info.common;
}

static struct rx_mpdu_start *
ath10k_rx_desc_wcn3990_get_mpdu_start(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->mpdu_start;
}

static struct rx_mpdu_end *
ath10k_rx_desc_wcn3990_get_mpdu_end(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->mpdu_end;
}

static struct rx_msdu_start_common *
ath10k_rx_desc_wcn3990_get_msdu_start(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->msdu_start.common;
}

static struct rx_msdu_end_common *
ath10k_rx_desc_wcn3990_get_msdu_end(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->msdu_end.common;
}

static struct rx_ppdu_start *
ath10k_rx_desc_wcn3990_get_ppdu_start(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->ppdu_start;
}

static struct rx_ppdu_end_common *
ath10k_rx_desc_wcn3990_get_ppdu_end(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return &rx_desc->ppdu_end.common;
}

static u8 *
ath10k_rx_desc_wcn3990_get_rx_hdr_status(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return rx_desc->rx_hdr_status;
}

static u8 *
ath10k_rx_desc_wcn3990_get_msdu_payload(struct htt_rx_desc *rxd)
{
	struct htt_rx_desc_v2 *rx_desc = container_of(rxd, struct htt_rx_desc_v2, base);

	return rx_desc->msdu_payload;
}

const struct ath10k_htt_rx_desc_ops wcn3990_rx_desc_ops = {
	.rx_desc_size = sizeof(struct htt_rx_desc_v2),
	.rx_desc_msdu_payload_offset = offsetof(struct htt_rx_desc_v2, msdu_payload),

	.rx_desc_from_raw_buffer = ath10k_rx_desc_wcn3990_from_raw_buffer,
	.rx_desc_get_offsets = ath10k_rx_desc_wcn3990_get_offsets,
	.rx_desc_get_attention = ath10k_rx_desc_wcn3990_get_attention,
	.rx_desc_get_frag_info = ath10k_rx_desc_wcn3990_get_frag_info,
	.rx_desc_get_mpdu_start = ath10k_rx_desc_wcn3990_get_mpdu_start,
	.rx_desc_get_mpdu_end = ath10k_rx_desc_wcn3990_get_mpdu_end,
	.rx_desc_get_msdu_start = ath10k_rx_desc_wcn3990_get_msdu_start,
	.rx_desc_get_msdu_end = ath10k_rx_desc_wcn3990_get_msdu_end,
	.rx_desc_get_ppdu_start = ath10k_rx_desc_wcn3990_get_ppdu_start,
	.rx_desc_get_ppdu_end = ath10k_rx_desc_wcn3990_get_ppdu_end,
	.rx_desc_get_rx_hdr_status = ath10k_rx_desc_wcn3990_get_rx_hdr_status,
	.rx_desc_get_msdu_payload = ath10k_rx_desc_wcn3990_get_msdu_payload,
};

int ath10k_htt_connect(struct ath10k_htt *htt)
{
	struct ath10k_htc_svc_conn_req conn_req;
	struct ath10k_htc_svc_conn_resp conn_resp;
	struct ath10k *ar = htt->ar;
	struct ath10k_htc_ep *ep;
	int status;

	memset(&conn_req, 0, sizeof(conn_req));
	memset(&conn_resp, 0, sizeof(conn_resp));

	conn_req.ep_ops.ep_tx_complete = ath10k_htt_htc_tx_complete;
	conn_req.ep_ops.ep_rx_complete = ath10k_htt_htc_t2h_msg_handler;
	conn_req.ep_ops.ep_tx_credits = ath10k_htt_op_ep_tx_credits;

	/* connect to control service */
	conn_req.service_id = ATH10K_HTC_SVC_ID_HTT_DATA_MSG;

	status = ath10k_htc_connect_service(&htt->ar->htc, &conn_req,
					    &conn_resp);

	if (status)
		return status;

	htt->eid = conn_resp.eid;

	if (ar->bus_param.dev_type == ATH10K_DEV_TYPE_HL) {
		ep = &ar->htc.endpoint[htt->eid];
		ath10k_htc_setup_tx_req(ep);
	}

	htt->disable_tx_comp = ath10k_hif_get_htt_tx_complete(htt->ar);
	if (htt->disable_tx_comp)
		ath10k_htc_change_tx_credit_flow(&htt->ar->htc, htt->eid, true);

	return 0;
}

int ath10k_htt_init(struct ath10k *ar)
{
	struct ath10k_htt *htt = &ar->htt;

	htt->ar = ar;

	/*
	 * Prefetch enough data to satisfy target
	 * classification engine.
	 * This is for LL chips. HL chips will probably
	 * transfer all frame in the tx fragment.
	 */
	htt->prefetch_len =
		36 + /* 802.11 + qos + ht */
		4 + /* 802.1q */
		8 + /* llc snap */
		2; /* ip4 dscp or ip6 priority */

	switch (ar->running_fw->fw_file.htt_op_version) {
	case ATH10K_FW_HTT_OP_VERSION_10_4:
		ar->htt.t2h_msg_types = htt_10_4_t2h_msg_types;
		ar->htt.t2h_msg_types_max = HTT_10_4_T2H_NUM_MSGS;
		break;
	case ATH10K_FW_HTT_OP_VERSION_10_1:
		ar->htt.t2h_msg_types = htt_10x_t2h_msg_types;
		ar->htt.t2h_msg_types_max = HTT_10X_T2H_NUM_MSGS;
		break;
	case ATH10K_FW_HTT_OP_VERSION_TLV:
		ar->htt.t2h_msg_types = htt_tlv_t2h_msg_types;
		ar->htt.t2h_msg_types_max = HTT_TLV_T2H_NUM_MSGS;
		break;
	case ATH10K_FW_HTT_OP_VERSION_MAIN:
		ar->htt.t2h_msg_types = htt_main_t2h_msg_types;
		ar->htt.t2h_msg_types_max = HTT_MAIN_T2H_NUM_MSGS;
		break;
	case ATH10K_FW_HTT_OP_VERSION_MAX:
	case ATH10K_FW_HTT_OP_VERSION_UNSET:
		WARN_ON(1);
		return -EINVAL;
	}
	ath10k_htt_set_tx_ops(htt);
	ath10k_htt_set_rx_ops(htt);

	return 0;
}

#define HTT_TARGET_VERSION_TIMEOUT_HZ (3 * HZ)

static int ath10k_htt_verify_version(struct ath10k_htt *htt)
{
	struct ath10k *ar = htt->ar;

	ath10k_dbg(ar, ATH10K_DBG_BOOT, "htt target version %d.%d\n",
		   htt->target_version_major, htt->target_version_minor);

	if (htt->target_version_major != 2 &&
	    htt->target_version_major != 3) {
		ath10k_err(ar, "unsupported htt major version %d. supported versions are 2 and 3\n",
			   htt->target_version_major);
		return -ENOTSUPP;
	}

	return 0;
}

int ath10k_htt_setup(struct ath10k_htt *htt)
{
	struct ath10k *ar = htt->ar;
	int status;

	init_completion(&htt->target_version_received);

	status = ath10k_htt_h2t_ver_req_msg(htt);
	if (status)
		return status;

	status = wait_for_completion_timeout(&htt->target_version_received,
					     HTT_TARGET_VERSION_TIMEOUT_HZ);
	if (status == 0) {
		ath10k_warn(ar, "htt version request timed out\n");
		return -ETIMEDOUT;
	}

	status = ath10k_htt_verify_version(htt);
	if (status) {
		ath10k_warn(ar, "failed to verify htt version: %d\n",
			    status);
		return status;
	}

	status = ath10k_htt_send_frag_desc_bank_cfg(htt);
	if (status)
		return status;

	status = ath10k_htt_send_rx_ring_cfg(htt);
	if (status) {
		ath10k_warn(ar, "failed to setup rx ring: %d\n",
			    status);
		return status;
	}

	status = ath10k_htt_h2t_aggr_cfg_msg(htt,
					     htt->max_num_ampdu,
					     htt->max_num_amsdu);
	if (status) {
		ath10k_warn(ar, "failed to setup amsdu/ampdu limit: %d\n",
			    status);
		return status;
	}

	return 0;
}
