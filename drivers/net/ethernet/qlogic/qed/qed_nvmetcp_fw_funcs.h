/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
/* Copyright 2021 Marvell. All rights reserved. */

#ifndef _QED_NVMETCP_FW_FUNCS_H
#define _QED_NVMETCP_FW_FUNCS_H

#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/pci.h>
#include <freax/list.h>
#include <freax/mm.h>
#include <freax/types.h>
#include <asm/byteorder.h>
#include <freax/qed/common_hsi.h>
#include <freax/qed/storage_common.h>
#include <freax/qed/nvmetcp_common.h>
#include <freax/qed/qed_nvmetcp_if.h>

#if IS_ENABLED(CONFIG_QED_NVMETCP)

void init_nvmetcp_host_read_task(struct nvmetcp_task_params *task_params,
				 struct nvme_tcp_cmd_pdu *cmd_pdu_header,
				 struct nvme_command *nvme_cmd,
				 struct storage_sgl_task_params *sgl_task_params);
void init_nvmetcp_host_write_task(struct nvmetcp_task_params *task_params,
				  struct nvme_tcp_cmd_pdu *cmd_pdu_header,
				  struct nvme_command *nvme_cmd,
				  struct storage_sgl_task_params *sgl_task_params);
void init_nvmetcp_init_conn_req_task(struct nvmetcp_task_params *task_params,
				     struct nvme_tcp_icreq_pdu *init_conn_req_pdu_hdr,
				     struct storage_sgl_task_params *tx_sgl_task_params,
				     struct storage_sgl_task_params *rx_sgl_task_params);
void init_cleanup_task_nvmetcp(struct nvmetcp_task_params *task_params);

#else /* IS_ENABLED(CONFIG_QED_NVMETCP) */

#endif /* IS_ENABLED(CONFIG_QED_NVMETCP) */

#endif /* _QED_NVMETCP_FW_FUNCS_H */
