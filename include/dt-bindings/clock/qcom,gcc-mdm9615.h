/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2013, The freax Foundation. All rights reserved.
 * Copyright (c) BayLibre, SAS.
 * Author : Neil Armstrong <narmstrong@baylibre.com>
 */

#ifndef _DT_BINDINGS_CLK_MDM_GCC_9615_H
#define _DT_BINDINGS_CLK_MDM_GCC_9615_H

#define AFAB_CLK_SRC				0
#define AFAB_CORE_CLK				1
#define SFAB_MSS_Q6_SW_A_CLK			2
#define SFAB_MSS_Q6_FW_A_CLK			3
#define QDSS_STM_CLK				4
#define SCSS_A_CLK				5
#define SCSS_H_CLK				6
#define SCSS_XO_SRC_CLK				7
#define AFAB_EBI1_CH0_A_CLK			8
#define AFAB_EBI1_CH1_A_CLK			9
#define AFAB_AXI_S0_FCLK			10
#define AFAB_AXI_S1_FCLK			11
#define AFAB_AXI_S2_FCLK			12
#define AFAB_AXI_S3_FCLK			13
#define AFAB_AXI_S4_FCLK			14
#define SFAB_CORE_CLK				15
#define SFAB_AXI_S0_FCLK			16
#define SFAB_AXI_S1_FCLK			17
#define SFAB_AXI_S2_FCLK			18
#define SFAB_AXI_S3_FCLK			19
#define SFAB_AXI_S4_FCLK			20
#define SFAB_AHB_S0_FCLK			21
#define SFAB_AHB_S1_FCLK			22
#define SFAB_AHB_S2_FCLK			23
#define SFAB_AHB_S3_FCLK			24
#define SFAB_AHB_S4_FCLK			25
#define SFAB_AHB_S5_FCLK			26
#define SFAB_AHB_S6_FCLK			27
#define SFAB_AHB_S7_FCLK			28
#define QDSS_AT_CLK_SRC				29
#define QDSS_AT_CLK				30
#define QDSS_TRACECLKIN_CLK_SRC			31
#define QDSS_TRACECLKIN_CLK			32
#define QDSS_TSCTR_CLK_SRC			33
#define QDSS_TSCTR_CLK				34
#define SFAB_ADM0_M0_A_CLK			35
#define SFAB_ADM0_M1_A_CLK			36
#define SFAB_ADM0_M2_H_CLK			37
#define ADM0_CLK				38
#define ADM0_PBUS_CLK				39
#define MSS_XPU_CLK				40
#define IMEM0_A_CLK				41
#define QDSS_H_CLK				42
#define PCIE_A_CLK				43
#define PCIE_AUX_CLK				44
#define PCIE_PHY_REF_CLK			45
#define PCIE_H_CLK				46
#define SFAB_CLK_SRC				47
#define MAHB0_CLK				48
#define Q6SW_CLK_SRC				49
#define Q6SW_CLK				50
#define Q6FW_CLK_SRC				51
#define Q6FW_CLK				52
#define SFAB_MSS_M_A_CLK			53
#define SFAB_USB3_M_A_CLK			54
#define SFAB_LPASS_Q6_A_CLK			55
#define SFAB_AFAB_M_A_CLK			56
#define AFAB_SFAB_M0_A_CLK			57
#define AFAB_SFAB_M1_A_CLK			58
#define SFAB_SATA_S_H_CLK			59
#define DFAB_CLK_SRC				60
#define DFAB_CLK				61
#define SFAB_DFAB_M_A_CLK			62
#define DFAB_SFAB_M_A_CLK			63
#define DFAB_SWAY0_H_CLK			64
#define DFAB_SWAY1_H_CLK			65
#define DFAB_ARB0_H_CLK				66
#define DFAB_ARB1_H_CLK				67
#define PPSS_H_CLK				68
#define PPSS_PROC_CLK				69
#define PPSS_TIMER0_CLK				70
#define PPSS_TIMER1_CLK				71
#define PMEM_A_CLK				72
#define DMA_BAM_H_CLK				73
#define SIC_H_CLK				74
#define SPS_TIC_H_CLK				75
#define SLIMBUS_H_CLK				76
#define SLIMBUS_XO_SRC_CLK			77
#define CFPB_2X_CLK_SRC				78
#define CFPB_CLK				79
#define CFPB0_H_CLK				80
#define CFPB1_H_CLK				81
#define CFPB2_H_CLK				82
#define SFAB_CFPB_M_H_CLK			83
#define CFPB_MASTER_H_CLK			84
#define SFAB_CFPB_S_H_CLK			85
#define CFPB_SPLITTER_H_CLK			86
#define TSIF_H_CLK				87
#define TSIF_INACTIVITY_TIMERS_CLK		88
#define TSIF_REF_SRC				89
#define TSIF_REF_CLK				90
#define CE1_H_CLK				91
#define CE1_CORE_CLK				92
#define CE1_SLEEP_CLK				93
#define CE2_H_CLK				94
#define CE2_CORE_CLK				95
#define SFPB_H_CLK_SRC				97
#define SFPB_H_CLK				98
#define SFAB_SFPB_M_H_CLK			99
#define SFAB_SFPB_S_H_CLK			100
#define RPM_PROC_CLK				101
#define RPM_BUS_H_CLK				102
#define RPM_SLEEP_CLK				103
#define RPM_TIMER_CLK				104
#define RPM_MSG_RAM_H_CLK			105
#define PMIC_ARB0_H_CLK				106
#define PMIC_ARB1_H_CLK				107
#define PMIC_SSBI2_SRC				108
#define PMIC_SSBI2_CLK				109
#define SDC1_H_CLK				110
#define SDC2_H_CLK				111
#define SDC3_H_CLK				112
#define SDC4_H_CLK				113
#define SDC5_H_CLK				114
#define SDC1_SRC				115
#define SDC2_SRC				116
#define SDC3_SRC				117
#define SDC4_SRC				118
#define SDC5_SRC				119
#define SDC1_CLK				120
#define SDC2_CLK				121
#define SDC3_CLK				122
#define SDC4_CLK				123
#define SDC5_CLK				124
#define DFAB_A2_H_CLK				125
#define USB_HS1_H_CLK				126
#define USB_HS1_XCVR_SRC			127
#define USB_HS1_XCVR_CLK			128
#define USB_HSIC_H_CLK				129
#define USB_HSIC_XCVR_FS_SRC			130
#define USB_HSIC_XCVR_FS_CLK			131
#define USB_HSIC_SYSTEM_CLK_SRC			132
#define USB_HSIC_SYSTEM_CLK			133
#define CFPB0_C0_H_CLK				134
#define CFPB0_C1_H_CLK				135
#define CFPB0_D0_H_CLK				136
#define CFPB0_D1_H_CLK				137
#define USB_FS1_H_CLK				138
#define USB_FS1_XCVR_FS_SRC			139
#define USB_FS1_XCVR_FS_CLK			140
#define USB_FS1_SYSTEM_CLK			141
#define USB_FS2_H_CLK				142
#define USB_FS2_XCVR_FS_SRC			143
#define USB_FS2_XCVR_FS_CLK			144
#define USB_FS2_SYSTEM_CLK			145
#define GSBI_COMMON_SIM_SRC			146
#define GSBI1_H_CLK				147
#define GSBI2_H_CLK				148
#define GSBI3_H_CLK				149
#define GSBI4_H_CLK				150
#define GSBI5_H_CLK				151
#define GSBI6_H_CLK				152
#define GSBI7_H_CLK				153
#define GSBI8_H_CLK				154
#define GSBI9_H_CLK				155
#define GSBI10_H_CLK				156
#define GSBI11_H_CLK				157
#define GSBI12_H_CLK				158
#define GSBI1_UART_SRC				159
#define GSBI1_UART_CLK				160
#define GSBI2_UART_SRC				161
#define GSBI2_UART_CLK				162
#define GSBI3_UART_SRC				163
#define GSBI3_UART_CLK				164
#define GSBI4_UART_SRC				165
#define GSBI4_UART_CLK				166
#define GSBI5_UART_SRC				167
#define GSBI5_UART_CLK				168
#define GSBI6_UART_SRC				169
#define GSBI6_UART_CLK				170
#define GSBI7_UART_SRC				171
#define GSBI7_UART_CLK				172
#define GSBI8_UART_SRC				173
#define GSBI8_UART_CLK				174
#define GSBI9_UART_SRC				175
#define GSBI9_UART_CLK				176
#define GSBI10_UART_SRC				177
#define GSBI10_UART_CLK				178
#define GSBI11_UART_SRC				179
#define GSBI11_UART_CLK				180
#define GSBI12_UART_SRC				181
#define GSBI12_UART_CLK				182
#define GSBI1_QUP_SRC				183
#define GSBI1_QUP_CLK				184
#define GSBI2_QUP_SRC				185
#define GSBI2_QUP_CLK				186
#define GSBI3_QUP_SRC				187
#define GSBI3_QUP_CLK				188
#define GSBI4_QUP_SRC				189
#define GSBI4_QUP_CLK				190
#define GSBI5_QUP_SRC				191
#define GSBI5_QUP_CLK				192
#define GSBI6_QUP_SRC				193
#define GSBI6_QUP_CLK				194
#define GSBI7_QUP_SRC				195
#define GSBI7_QUP_CLK				196
#define GSBI8_QUP_SRC				197
#define GSBI8_QUP_CLK				198
#define GSBI9_QUP_SRC				199
#define GSBI9_QUP_CLK				200
#define GSBI10_QUP_SRC				201
#define GSBI10_QUP_CLK				202
#define GSBI11_QUP_SRC				203
#define GSBI11_QUP_CLK				204
#define GSBI12_QUP_SRC				205
#define GSBI12_QUP_CLK				206
#define GSBI1_SIM_CLK				207
#define GSBI2_SIM_CLK				208
#define GSBI3_SIM_CLK				209
#define GSBI4_SIM_CLK				210
#define GSBI5_SIM_CLK				211
#define GSBI6_SIM_CLK				212
#define GSBI7_SIM_CLK				213
#define GSBI8_SIM_CLK				214
#define GSBI9_SIM_CLK				215
#define GSBI10_SIM_CLK				216
#define GSBI11_SIM_CLK				217
#define GSBI12_SIM_CLK				218
#define USB_HSIC_HSIC_CLK_SRC			219
#define USB_HSIC_HSIC_CLK			220
#define USB_HSIC_HSIO_CAL_CLK			221
#define SPDM_CFG_H_CLK				222
#define SPDM_MSTR_H_CLK				223
#define SPDM_FF_CLK_SRC				224
#define SPDM_FF_CLK				225
#define SEC_CTRL_CLK				226
#define SEC_CTRL_ACC_CLK_SRC			227
#define SEC_CTRL_ACC_CLK			228
#define TLMM_H_CLK				229
#define TLMM_CLK				230
#define SFAB_MSS_S_H_CLK			231
#define MSS_SLP_CLK				232
#define MSS_Q6SW_JTAG_CLK			233
#define MSS_Q6FW_JTAG_CLK			234
#define MSS_S_H_CLK				235
#define MSS_CXO_SRC_CLK				236
#define SATA_H_CLK				237
#define SATA_CLK_SRC				238
#define SATA_RXOOB_CLK				239
#define SATA_PMALIVE_CLK			240
#define SATA_PHY_REF_CLK			241
#define TSSC_CLK_SRC				242
#define TSSC_CLK				243
#define PDM_SRC					244
#define PDM_CLK					245
#define GP0_SRC					246
#define GP0_CLK					247
#define GP1_SRC					248
#define GP1_CLK					249
#define GP2_SRC					250
#define GP2_CLK					251
#define MPM_CLK					252
#define EBI1_CLK_SRC				253
#define EBI1_CH0_CLK				254
#define EBI1_CH1_CLK				255
#define EBI1_2X_CLK				256
#define EBI1_CH0_DQ_CLK				257
#define EBI1_CH1_DQ_CLK				258
#define EBI1_CH0_CA_CLK				259
#define EBI1_CH1_CA_CLK				260
#define EBI1_XO_CLK				261
#define SFAB_SMPSS_S_H_CLK			262
#define PRNG_SRC				263
#define PRNG_CLK				264
#define PXO_SRC					265
#define LPASS_CXO_CLK				266
#define LPASS_PXO_CLK				267
#define SPDM_CY_PORT0_CLK			268
#define SPDM_CY_PORT1_CLK			269
#define SPDM_CY_PORT2_CLK			270
#define SPDM_CY_PORT3_CLK			271
#define SPDM_CY_PORT4_CLK			272
#define SPDM_CY_PORT5_CLK			273
#define SPDM_CY_PORT6_CLK			274
#define SPDM_CY_PORT7_CLK			275
#define PLL0					276
#define PLL0_VOTE				277
#define PLL3					278
#define PLL3_VOTE				279
#define PLL4_VOTE				280
#define PLL5					281
#define PLL5_VOTE				282
#define PLL6					283
#define PLL6_VOTE				284
#define PLL7_VOTE				285
#define PLL8					286
#define PLL8_VOTE				287
#define PLL9					288
#define PLL10					289
#define PLL11					290
#define PLL12					291
#define PLL13					292
#define PLL14					293
#define PLL14_VOTE				294
#define USB_HS3_H_CLK				295
#define USB_HS3_XCVR_SRC			296
#define USB_HS3_XCVR_CLK			297
#define USB_HS4_H_CLK				298
#define USB_HS4_XCVR_SRC			299
#define USB_HS4_XCVR_CLK			300
#define SATA_PHY_CFG_CLK			301
#define SATA_A_CLK				302
#define CE3_SRC					303
#define CE3_CORE_CLK				304
#define CE3_H_CLK				305
#define USB_HS1_SYSTEM_CLK_SRC			306
#define USB_HS1_SYSTEM_CLK			307
#define EBI2_CLK				308
#define EBI2_AON_CLK				309

#endif
