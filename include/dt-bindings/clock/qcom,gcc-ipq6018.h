/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2018, The freax Foundation. All rights reserved.
 */

#ifndef _DT_BINDINGS_CLOCK_IPQ_GCC_6018_H
#define _DT_BINDINGS_CLOCK_IPQ_GCC_6018_H

#define GPLL0					0
#define UBI32_PLL				1
#define GPLL6					2
#define GPLL4					3
#define PCNOC_BFDCD_CLK_SRC			4
#define GPLL2					5
#define NSS_CRYPTO_PLL				6
#define NSS_PPE_CLK_SRC				7
#define GCC_XO_CLK_SRC				8
#define NSS_CE_CLK_SRC				9
#define GCC_SLEEP_CLK_SRC			10
#define APSS_AHB_CLK_SRC			11
#define NSS_PORT5_RX_CLK_SRC			12
#define NSS_PORT5_TX_CLK_SRC			13
#define PCIE0_AXI_CLK_SRC			14
#define USB0_MASTER_CLK_SRC			15
#define APSS_AHB_POSTDIV_CLK_SRC		16
#define NSS_PORT1_RX_CLK_SRC			17
#define NSS_PORT1_TX_CLK_SRC			18
#define NSS_PORT2_RX_CLK_SRC			19
#define NSS_PORT2_TX_CLK_SRC			20
#define NSS_PORT3_RX_CLK_SRC			21
#define NSS_PORT3_TX_CLK_SRC			22
#define NSS_PORT4_RX_CLK_SRC			23
#define NSS_PORT4_TX_CLK_SRC			24
#define NSS_PORT5_RX_DIV_CLK_SRC		25
#define NSS_PORT5_TX_DIV_CLK_SRC		26
#define APSS_AXI_CLK_SRC			27
#define NSS_CRYPTO_CLK_SRC			28
#define NSS_PORT1_RX_DIV_CLK_SRC		29
#define NSS_PORT1_TX_DIV_CLK_SRC		30
#define NSS_PORT2_RX_DIV_CLK_SRC		31
#define NSS_PORT2_TX_DIV_CLK_SRC		32
#define NSS_PORT3_RX_DIV_CLK_SRC		33
#define NSS_PORT3_TX_DIV_CLK_SRC		34
#define NSS_PORT4_RX_DIV_CLK_SRC		35
#define NSS_PORT4_TX_DIV_CLK_SRC		36
#define NSS_UBI0_CLK_SRC			37
#define BLSP1_QUP1_I2C_APPS_CLK_SRC		38
#define BLSP1_QUP1_SPI_APPS_CLK_SRC		39
#define BLSP1_QUP2_I2C_APPS_CLK_SRC		40
#define BLSP1_QUP2_SPI_APPS_CLK_SRC		41
#define BLSP1_QUP3_I2C_APPS_CLK_SRC		42
#define BLSP1_QUP3_SPI_APPS_CLK_SRC		43
#define BLSP1_QUP4_I2C_APPS_CLK_SRC		44
#define BLSP1_QUP4_SPI_APPS_CLK_SRC		45
#define BLSP1_QUP5_I2C_APPS_CLK_SRC		46
#define BLSP1_QUP5_SPI_APPS_CLK_SRC		47
#define BLSP1_QUP6_I2C_APPS_CLK_SRC		48
#define BLSP1_QUP6_SPI_APPS_CLK_SRC		49
#define BLSP1_UART1_APPS_CLK_SRC		50
#define BLSP1_UART2_APPS_CLK_SRC		51
#define BLSP1_UART3_APPS_CLK_SRC		52
#define BLSP1_UART4_APPS_CLK_SRC		53
#define BLSP1_UART5_APPS_CLK_SRC		54
#define BLSP1_UART6_APPS_CLK_SRC		55
#define CRYPTO_CLK_SRC				56
#define NSS_UBI0_DIV_CLK_SRC			57
#define PCIE0_AUX_CLK_SRC			58
#define PCIE0_PIPE_CLK_SRC			59
#define SDCC1_APPS_CLK_SRC			60
#define USB0_AUX_CLK_SRC			61
#define USB0_MOCK_UTMI_CLK_SRC			62
#define USB0_PIPE_CLK_SRC			63
#define USB1_MOCK_UTMI_CLK_SRC			64
#define GCC_APSS_AHB_CLK			65
#define GCC_APSS_AXI_CLK			66
#define GCC_BLSP1_AHB_CLK			67
#define GCC_BLSP1_QUP1_I2C_APPS_CLK		68
#define GCC_BLSP1_QUP1_SPI_APPS_CLK		69
#define GCC_BLSP1_QUP2_I2C_APPS_CLK		70
#define GCC_BLSP1_QUP2_SPI_APPS_CLK		71
#define GCC_BLSP1_QUP3_I2C_APPS_CLK		72
#define GCC_BLSP1_QUP3_SPI_APPS_CLK		73
#define GCC_BLSP1_QUP4_I2C_APPS_CLK		74
#define GCC_BLSP1_QUP4_SPI_APPS_CLK		75
#define GCC_BLSP1_QUP5_I2C_APPS_CLK		76
#define GCC_BLSP1_QUP5_SPI_APPS_CLK		77
#define GCC_BLSP1_QUP6_I2C_APPS_CLK		78
#define GCC_BLSP1_QUP6_SPI_APPS_CLK		79
#define GCC_BLSP1_UART1_APPS_CLK		80
#define GCC_BLSP1_UART2_APPS_CLK		81
#define GCC_BLSP1_UART3_APPS_CLK		82
#define GCC_BLSP1_UART4_APPS_CLK		83
#define GCC_BLSP1_UART5_APPS_CLK		84
#define GCC_BLSP1_UART6_APPS_CLK		85
#define GCC_CRYPTO_AHB_CLK			86
#define GCC_CRYPTO_AXI_CLK			87
#define GCC_CRYPTO_CLK				88
#define GCC_XO_CLK				89
#define GCC_XO_DIV4_CLK				90
#define GCC_MDIO_AHB_CLK			91
#define GCC_CRYPTO_PPE_CLK			92
#define GCC_NSS_CE_APB_CLK			93
#define GCC_NSS_CE_AXI_CLK			94
#define GCC_NSS_CFG_CLK				95
#define GCC_NSS_CRYPTO_CLK			96
#define GCC_NSS_CSR_CLK				97
#define GCC_NSS_EDMA_CFG_CLK			98
#define GCC_NSS_EDMA_CLK			99
#define GCC_NSS_NOC_CLK				100
#define GCC_NSS_PORT1_RX_CLK			101
#define GCC_NSS_PORT1_TX_CLK			102
#define GCC_NSS_PORT2_RX_CLK			103
#define GCC_NSS_PORT2_TX_CLK			104
#define GCC_NSS_PORT3_RX_CLK			105
#define GCC_NSS_PORT3_TX_CLK			106
#define GCC_NSS_PORT4_RX_CLK			107
#define GCC_NSS_PORT4_TX_CLK			108
#define GCC_NSS_PORT5_RX_CLK			109
#define GCC_NSS_PORT5_TX_CLK			110
#define GCC_NSS_PPE_CFG_CLK			111
#define GCC_NSS_PPE_CLK				112
#define GCC_NSS_PPE_IPE_CLK			113
#define GCC_NSS_PTP_REF_CLK			114
#define GCC_NSSNOC_CE_APB_CLK			115
#define GCC_NSSNOC_CE_AXI_CLK			116
#define GCC_NSSNOC_CRYPTO_CLK			117
#define GCC_NSSNOC_PPE_CFG_CLK			118
#define GCC_NSSNOC_PPE_CLK			119
#define GCC_NSSNOC_QOSGEN_REF_CLK		120
#define GCC_NSSNOC_TIMEOUT_REF_CLK		121
#define GCC_NSSNOC_UBI0_AHB_CLK			122
#define GCC_PORT1_MAC_CLK			123
#define GCC_PORT2_MAC_CLK			124
#define GCC_PORT3_MAC_CLK			125
#define GCC_PORT4_MAC_CLK			126
#define GCC_PORT5_MAC_CLK			127
#define GCC_UBI0_AHB_CLK			128
#define GCC_UBI0_AXI_CLK			129
#define GCC_UBI0_CORE_CLK			130
#define GCC_PCIE0_AHB_CLK			131
#define GCC_PCIE0_AUX_CLK			132
#define GCC_PCIE0_AXI_M_CLK			133
#define GCC_PCIE0_AXI_S_CLK			134
#define GCC_PCIE0_PIPE_CLK			135
#define GCC_PRNG_AHB_CLK			136
#define GCC_QPIC_AHB_CLK			137
#define GCC_QPIC_CLK				138
#define GCC_SDCC1_AHB_CLK			139
#define GCC_SDCC1_APPS_CLK			140
#define GCC_UNIPHY0_AHB_CLK			141
#define GCC_UNIPHY0_PORT1_RX_CLK		142
#define GCC_UNIPHY0_PORT1_TX_CLK		143
#define GCC_UNIPHY0_PORT2_RX_CLK		144
#define GCC_UNIPHY0_PORT2_TX_CLK		145
#define GCC_UNIPHY0_PORT3_RX_CLK		146
#define GCC_UNIPHY0_PORT3_TX_CLK		147
#define GCC_UNIPHY0_PORT4_RX_CLK		148
#define GCC_UNIPHY0_PORT4_TX_CLK		149
#define GCC_UNIPHY0_PORT5_RX_CLK		150
#define GCC_UNIPHY0_PORT5_TX_CLK		151
#define GCC_UNIPHY0_SYS_CLK			152
#define GCC_UNIPHY1_AHB_CLK			153
#define GCC_UNIPHY1_PORT5_RX_CLK		154
#define GCC_UNIPHY1_PORT5_TX_CLK		155
#define GCC_UNIPHY1_SYS_CLK			156
#define GCC_USB0_AUX_CLK			157
#define GCC_USB0_MASTER_CLK			158
#define GCC_USB0_MOCK_UTMI_CLK			159
#define GCC_USB0_PHY_CFG_AHB_CLK		160
#define GCC_USB0_PIPE_CLK			161
#define GCC_USB0_SLEEP_CLK			162
#define GCC_USB1_MASTER_CLK			163
#define GCC_USB1_MOCK_UTMI_CLK			164
#define GCC_USB1_PHY_CFG_AHB_CLK		165
#define GCC_USB1_SLEEP_CLK			166
#define GP1_CLK_SRC				167
#define GP2_CLK_SRC				168
#define GP3_CLK_SRC				169
#define GCC_GP1_CLK				170
#define GCC_GP2_CLK				171
#define GCC_GP3_CLK				172
#define SYSTEM_NOC_BFDCD_CLK_SRC		173
#define GCC_NSSNOC_SNOC_CLK			174
#define GCC_UBI0_NC_AXI_CLK			175
#define GCC_UBI1_NC_AXI_CLK			176
#define GPLL0_MAIN				177
#define UBI32_PLL_MAIN				178
#define GPLL6_MAIN				179
#define GPLL4_MAIN				180
#define GPLL2_MAIN				181
#define NSS_CRYPTO_PLL_MAIN			182
#define GCC_CMN_12GPLL_AHB_CLK			183
#define GCC_CMN_12GPLL_SYS_CLK			184
#define GCC_SNOC_BUS_TIMEOUT2_AHB_CLK		185
#define GCC_SYS_NOC_USB0_AXI_CLK		186
#define GCC_SYS_NOC_PCIE0_AXI_CLK		187
#define QDSS_TSCTR_CLK_SRC			188
#define QDSS_AT_CLK_SRC				189
#define GCC_QDSS_AT_CLK				190
#define GCC_QDSS_DAP_CLK			191
#define ADSS_PWM_CLK_SRC			192
#define GCC_ADSS_PWM_CLK			193
#define SDCC1_ICE_CORE_CLK_SRC			194
#define GCC_SDCC1_ICE_CORE_CLK			195
#define GCC_DCC_CLK				196
#define PCIE0_RCHNG_CLK_SRC			197
#define GCC_PCIE0_AXI_S_BRIDGE_CLK		198
#define PCIE0_RCHNG_CLK				199
#define UBI32_MEM_NOC_BFDCD_CLK_SRC		200
#define WCSS_AHB_CLK_SRC			201
#define Q6_AXI_CLK_SRC				202
#define GCC_Q6SS_PCLKDBG_CLK			203
#define GCC_Q6_TSCTR_1TO2_CLK			204
#define GCC_WCSS_CORE_TBU_CLK			205
#define GCC_WCSS_AXI_M_CLK			206
#define GCC_SYS_NOC_WCSS_AHB_CLK		207
#define GCC_Q6_AXIM_CLK				208
#define GCC_Q6SS_ATBM_CLK			209
#define GCC_WCSS_Q6_TBU_CLK			210
#define GCC_Q6_AXIM2_CLK			211
#define GCC_Q6_AHB_CLK				212
#define GCC_Q6_AHB_S_CLK			213
#define GCC_WCSS_DBG_IFC_APB_CLK		214
#define GCC_WCSS_DBG_IFC_ATB_CLK		215
#define GCC_WCSS_DBG_IFC_NTS_CLK		216
#define GCC_WCSS_DBG_IFC_DAPBUS_CLK		217
#define GCC_WCSS_DBG_IFC_APB_BDG_CLK		218
#define GCC_WCSS_DBG_IFC_ATB_BDG_CLK		219
#define GCC_WCSS_DBG_IFC_NTS_BDG_CLK		220
#define GCC_WCSS_DBG_IFC_DAPBUS_BDG_CLK		221
#define GCC_WCSS_ECAHB_CLK			222
#define GCC_WCSS_ACMT_CLK			223
#define GCC_WCSS_AHB_S_CLK			224
#define GCC_RBCPR_WCSS_CLK			225
#define RBCPR_WCSS_CLK_SRC			226
#define GCC_RBCPR_WCSS_AHB_CLK			227
#define GCC_LPASS_CORE_AXIM_CLK			228
#define GCC_LPASS_SNOC_CFG_CLK			229
#define GCC_LPASS_Q6_AXIM_CLK			230
#define GCC_LPASS_Q6_ATBM_AT_CLK		231
#define GCC_LPASS_Q6_PCLKDBG_CLK		232
#define GCC_LPASS_Q6SS_TSCTR_1TO2_CLK		233
#define GCC_LPASS_Q6SS_TRIG_CLK			234
#define GCC_LPASS_TBU_CLK			235
#define LPASS_CORE_AXIM_CLK_SRC			236
#define LPASS_SNOC_CFG_CLK_SRC			237
#define LPASS_Q6_AXIM_CLK_SRC			238
#define GCC_PCNOC_LPASS_CLK			239
#define GCC_UBI0_UTCM_CLK			240
#define SNOC_NSSNOC_BFDCD_CLK_SRC		241
#define GCC_SNOC_NSSNOC_CLK			242
#define GCC_MEM_NOC_Q6_AXI_CLK			243
#define GCC_MEM_NOC_UBI32_CLK			244
#define GCC_MEM_NOC_LPASS_CLK			245
#define GCC_SNOC_LPASS_CFG_CLK			246
#define GCC_SYS_NOC_QDSS_STM_AXI_CLK		247
#define GCC_QDSS_STM_CLK			248
#define GCC_QDSS_TRACECLKIN_CLK			249
#define QDSS_STM_CLK_SRC			250
#define QDSS_TRACECLKIN_CLK_SRC			251
#define GCC_NSSNOC_ATB_CLK			252
#endif
