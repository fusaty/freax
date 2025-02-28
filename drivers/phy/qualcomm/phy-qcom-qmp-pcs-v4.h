/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017, The freax Foundation. All rights reserved.
 */

#ifndef QCOM_PHY_QMP_PCS_V4_H_
#define QCOM_PHY_QMP_PCS_V4_H_

/* Only for QMP V4 PHY - USB/PCIe PCS registers */
#define QPHY_V4_PCS_SW_RESET				0x000
#define QPHY_V4_PCS_REVISION_ID0			0x004
#define QPHY_V4_PCS_REVISION_ID1			0x008
#define QPHY_V4_PCS_REVISION_ID2			0x00c
#define QPHY_V4_PCS_REVISION_ID3			0x010
#define QPHY_V4_PCS_PCS_STATUS1				0x014
#define QPHY_V4_PCS_PCS_STATUS2				0x018
#define QPHY_V4_PCS_PCS_STATUS3				0x01c
#define QPHY_V4_PCS_PCS_STATUS4				0x020
#define QPHY_V4_PCS_PCS_STATUS5				0x024
#define QPHY_V4_PCS_PCS_STATUS6				0x028
#define QPHY_V4_PCS_PCS_STATUS7				0x02c
#define QPHY_V4_PCS_DEBUG_BUS_0_STATUS			0x030
#define QPHY_V4_PCS_DEBUG_BUS_1_STATUS			0x034
#define QPHY_V4_PCS_DEBUG_BUS_2_STATUS			0x038
#define QPHY_V4_PCS_DEBUG_BUS_3_STATUS			0x03c
#define QPHY_V4_PCS_POWER_DOWN_CONTROL			0x040
#define QPHY_V4_PCS_START_CONTROL			0x044
#define QPHY_V4_PCS_INSIG_SW_CTRL1			0x048
#define QPHY_V4_PCS_INSIG_SW_CTRL2			0x04c
#define QPHY_V4_PCS_INSIG_SW_CTRL3			0x050
#define QPHY_V4_PCS_INSIG_SW_CTRL4			0x054
#define QPHY_V4_PCS_INSIG_SW_CTRL5			0x058
#define QPHY_V4_PCS_INSIG_SW_CTRL6			0x05c
#define QPHY_V4_PCS_INSIG_SW_CTRL7			0x060
#define QPHY_V4_PCS_INSIG_SW_CTRL8			0x064
#define QPHY_V4_PCS_INSIG_MX_CTRL1			0x068
#define QPHY_V4_PCS_INSIG_MX_CTRL2			0x06c
#define QPHY_V4_PCS_INSIG_MX_CTRL3			0x070
#define QPHY_V4_PCS_INSIG_MX_CTRL4			0x074
#define QPHY_V4_PCS_INSIG_MX_CTRL5			0x078
#define QPHY_V4_PCS_INSIG_MX_CTRL7			0x07c
#define QPHY_V4_PCS_INSIG_MX_CTRL8			0x080
#define QPHY_V4_PCS_OUTSIG_SW_CTRL1			0x084
#define QPHY_V4_PCS_OUTSIG_MX_CTRL1			0x088
#define QPHY_V4_PCS_CLAMP_ENABLE			0x08c
#define QPHY_V4_PCS_POWER_STATE_CONFIG1			0x090
#define QPHY_V4_PCS_POWER_STATE_CONFIG2			0x094
#define QPHY_V4_PCS_FLL_CNTRL1				0x098
#define QPHY_V4_PCS_FLL_CNTRL2				0x09c
#define QPHY_V4_PCS_FLL_CNT_VAL_L			0x0a0
#define QPHY_V4_PCS_FLL_CNT_VAL_H_TOL			0x0a4
#define QPHY_V4_PCS_FLL_MAN_CODE			0x0a8
#define QPHY_V4_PCS_TEST_CONTROL1			0x0ac
#define QPHY_V4_PCS_TEST_CONTROL2			0x0b0
#define QPHY_V4_PCS_TEST_CONTROL3			0x0b4
#define QPHY_V4_PCS_TEST_CONTROL4			0x0b8
#define QPHY_V4_PCS_TEST_CONTROL5			0x0bc
#define QPHY_V4_PCS_TEST_CONTROL6			0x0c0
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG1			0x0c4
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG2			0x0c8
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG3			0x0cc
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG4			0x0d0
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG5			0x0d4
#define QPHY_V4_PCS_LOCK_DETECT_CONFIG6			0x0d8
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG1			0x0dc
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG2			0x0e0
#define QPHY_V4_PCS_REFGEN_REQ_CONFIG3			0x0e4
#define QPHY_V4_PCS_BIST_CTRL				0x0e8
#define QPHY_V4_PCS_PRBS_POLY0				0x0ec
#define QPHY_V4_PCS_PRBS_POLY1				0x0f0
#define QPHY_V4_PCS_FIXED_PAT0				0x0f4
#define QPHY_V4_PCS_FIXED_PAT1				0x0f8
#define QPHY_V4_PCS_FIXED_PAT2				0x0fc
#define QPHY_V4_PCS_FIXED_PAT3				0x100
#define QPHY_V4_PCS_FIXED_PAT4				0x104
#define QPHY_V4_PCS_FIXED_PAT5				0x108
#define QPHY_V4_PCS_FIXED_PAT6				0x10c
#define QPHY_V4_PCS_FIXED_PAT7				0x110
#define QPHY_V4_PCS_FIXED_PAT8				0x114
#define QPHY_V4_PCS_FIXED_PAT9				0x118
#define QPHY_V4_PCS_FIXED_PAT10				0x11c
#define QPHY_V4_PCS_FIXED_PAT11				0x120
#define QPHY_V4_PCS_FIXED_PAT12				0x124
#define QPHY_V4_PCS_FIXED_PAT13				0x128
#define QPHY_V4_PCS_FIXED_PAT14				0x12c
#define QPHY_V4_PCS_FIXED_PAT15				0x130
#define QPHY_V4_PCS_TXMGN_CONFIG			0x134
#define QPHY_V4_PCS_G12S1_TXMGN_V0			0x138
#define QPHY_V4_PCS_G12S1_TXMGN_V1			0x13c
#define QPHY_V4_PCS_G12S1_TXMGN_V2			0x140
#define QPHY_V4_PCS_G12S1_TXMGN_V3			0x144
#define QPHY_V4_PCS_G12S1_TXMGN_V4			0x148
#define QPHY_V4_PCS_G12S1_TXMGN_V0_RS			0x14c
#define QPHY_V4_PCS_G12S1_TXMGN_V1_RS			0x150
#define QPHY_V4_PCS_G12S1_TXMGN_V2_RS			0x154
#define QPHY_V4_PCS_G12S1_TXMGN_V3_RS			0x158
#define QPHY_V4_PCS_G12S1_TXMGN_V4_RS			0x15c
#define QPHY_V4_PCS_G3S2_TXMGN_MAIN			0x160
#define QPHY_V4_PCS_G3S2_TXMGN_MAIN_RS			0x164
#define QPHY_V4_PCS_G12S1_TXDEEMPH_M6DB			0x168
#define QPHY_V4_PCS_G12S1_TXDEEMPH_M3P5DB		0x16c
#define QPHY_V4_PCS_G3S2_PRE_GAIN			0x170
#define QPHY_V4_PCS_G3S2_POST_GAIN			0x174
#define QPHY_V4_PCS_G3S2_PRE_POST_OFFSET		0x178
#define QPHY_V4_PCS_G3S2_PRE_GAIN_RS			0x17c
#define QPHY_V4_PCS_G3S2_POST_GAIN_RS			0x180
#define QPHY_V4_PCS_G3S2_PRE_POST_OFFSET_RS		0x184
#define QPHY_V4_PCS_RX_SIGDET_LVL			0x188
#define QPHY_V4_PCS_RX_SIGDET_DTCT_CNTRL		0x18c
#define QPHY_V4_PCS_RCVR_DTCT_DLY_P1U2_L		0x190
#define QPHY_V4_PCS_RCVR_DTCT_DLY_P1U2_H		0x194
#define QPHY_V4_PCS_RATE_SLEW_CNTRL1			0x198
#define QPHY_V4_PCS_RATE_SLEW_CNTRL2			0x19c
#define QPHY_V4_PCS_PWRUP_RESET_DLY_TIME_AUXCLK		0x1a0
#define QPHY_V4_PCS_P2U3_WAKEUP_DLY_TIME_AUXCLK_L	0x1a4
#define QPHY_V4_PCS_P2U3_WAKEUP_DLY_TIME_AUXCLK_H	0x1a8
#define QPHY_V4_PCS_TSYNC_RSYNC_TIME			0x1ac
#define QPHY_V4_PCS_CDR_RESET_TIME			0x1b0
#define QPHY_V4_PCS_TSYNC_DLY_TIME			0x1b4
#define QPHY_V4_PCS_ELECIDLE_DLY_SEL			0x1b8
#define QPHY_V4_PCS_CMN_ACK_OUT_SEL			0x1bc
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG1		0x1c0
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG2		0x1c4
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG3		0x1c8
#define QPHY_V4_PCS_ALIGN_DETECT_CONFIG4		0x1cc
#define QPHY_V4_PCS_PCS_TX_RX_CONFIG			0x1d0
#define QPHY_V4_PCS_RX_IDLE_DTCT_CNTRL			0x1d4
#define QPHY_V4_PCS_RX_DCC_CAL_CONFIG			0x1d8
#define QPHY_V4_PCS_EQ_CONFIG1				0x1dc
#define QPHY_V4_PCS_EQ_CONFIG2				0x1e0
#define QPHY_V4_PCS_EQ_CONFIG3				0x1e4
#define QPHY_V4_PCS_EQ_CONFIG4				0x1e8
#define QPHY_V4_PCS_EQ_CONFIG5				0x1ec

#endif
