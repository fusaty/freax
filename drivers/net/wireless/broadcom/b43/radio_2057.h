/* SPDX-License-Identifier: GPL-2.0 */
#ifndef B43_RADIO_2057_H_
#define B43_RADIO_2057_H_

#include <freax/types.h>

#include "tables_nphy.h"

#define R2057_DACBUF_VINCM_CORE0		0x000
#define R2057_IDCODE				0x001
#define R2057_RCCAL_MASTER			0x002
#define R2057_RCCAL_CAP_SIZE			0x003
#define R2057_RCAL_CONFIG			0x004
#define R2057_GPAIO_CONFIG			0x005
#define R2057_GPAIO_SEL1			0x006
#define R2057_GPAIO_SEL0			0x007
#define R2057_CLPO_CONFIG			0x008
#define R2057_BANDGAP_CONFIG			0x009
#define R2057_BANDGAP_RCAL_TRIM			0x00a
#define R2057_AFEREG_CONFIG			0x00b
#define R2057_TEMPSENSE_CONFIG			0x00c
#define R2057_XTAL_CONFIG1			0x00d
#define R2057_XTAL_ICORE_SIZE			0x00e
#define R2057_XTAL_BUF_SIZE			0x00f
#define R2057_XTAL_PULLCAP_SIZE			0x010
#define R2057_RFPLL_MASTER			0x011
#define R2057_VCOMONITOR_VTH_L			0x012
#define R2057_VCOMONITOR_VTH_H			0x013
#define R2057_VCOCAL_BIASRESET_RFPLLREG_VOUT	0x014
#define R2057_VCO_VARCSIZE_IDAC			0x015
#define R2057_VCOCAL_COUNTVAL0			0x016
#define R2057_VCOCAL_COUNTVAL1			0x017
#define R2057_VCOCAL_INTCLK_COUNT		0x018
#define R2057_VCOCAL_MASTER			0x019
#define R2057_VCOCAL_NUMCAPCHANGE		0x01a
#define R2057_VCOCAL_WINSIZE			0x01b
#define R2057_VCOCAL_DELAY_AFTER_REFRESH	0x01c
#define R2057_VCOCAL_DELAY_AFTER_CLOSELOOP	0x01d
#define R2057_VCOCAL_DELAY_AFTER_OPENLOOP	0x01e
#define R2057_VCOCAL_DELAY_BEFORE_OPENLOOP	0x01f
#define R2057_VCO_FORCECAPEN_FORCECAP1		0x020
#define R2057_VCO_FORCECAP0			0x021
#define R2057_RFPLL_REFMASTER_SPAREXTALSIZE	0x022
#define R2057_RFPLL_PFD_RESET_PW		0x023
#define R2057_RFPLL_LOOPFILTER_R2		0x024
#define R2057_RFPLL_LOOPFILTER_R1		0x025
#define R2057_RFPLL_LOOPFILTER_C3		0x026
#define R2057_RFPLL_LOOPFILTER_C2		0x027
#define R2057_RFPLL_LOOPFILTER_C1		0x028
#define R2057_CP_KPD_IDAC			0x029
#define R2057_RFPLL_IDACS			0x02a
#define R2057_RFPLL_MISC_EN			0x02b
#define R2057_RFPLL_MMD0			0x02c
#define R2057_RFPLL_MMD1			0x02d
#define R2057_RFPLL_MISC_CAL_RESETN		0x02e
#define R2057_JTAGXTAL_SIZE_CPBIAS_FILTRES	0x02f
#define R2057_VCO_ALCREF_BBPLLXTAL_SIZE		0x030
#define R2057_VCOCAL_READCAP0			0x031
#define R2057_VCOCAL_READCAP1			0x032
#define R2057_VCOCAL_STATUS			0x033
#define R2057_LOGEN_PUS				0x034
#define R2057_LOGEN_PTAT_RESETS			0x035
#define R2057_VCOBUF_IDACS			0x036
#define R2057_VCOBUF_TUNE			0x037
#define R2057_CMOSBUF_TX2GQ_IDACS		0x038
#define R2057_CMOSBUF_TX2GI_IDACS		0x039
#define R2057_CMOSBUF_TX5GQ_IDACS		0x03a
#define R2057_CMOSBUF_TX5GI_IDACS		0x03b
#define R2057_CMOSBUF_RX2GQ_IDACS		0x03c
#define R2057_CMOSBUF_RX2GI_IDACS		0x03d
#define R2057_CMOSBUF_RX5GQ_IDACS		0x03e
#define R2057_CMOSBUF_RX5GI_IDACS		0x03f
#define R2057_LOGEN_MX2G_IDACS			0x040
#define R2057_LOGEN_MX2G_TUNE			0x041
#define R2057_LOGEN_MX5G_IDACS			0x042
#define R2057_LOGEN_MX5G_TUNE			0x043
#define R2057_LOGEN_MX5G_RCCR			0x044
#define R2057_LOGEN_INDBUF2G_IDAC		0x045
#define R2057_LOGEN_INDBUF2G_IBOOST		0x046
#define R2057_LOGEN_INDBUF2G_TUNE		0x047
#define R2057_LOGEN_INDBUF5G_IDAC		0x048
#define R2057_LOGEN_INDBUF5G_IBOOST		0x049
#define R2057_LOGEN_INDBUF5G_TUNE		0x04a
#define R2057_CMOSBUF_TX_RCCR			0x04b
#define R2057_CMOSBUF_RX_RCCR			0x04c
#define R2057_LOGEN_SEL_PKDET			0x04d
#define R2057_CMOSBUF_SHAREIQ_PTAT		0x04e

/* MISC core 0 */
#define R2057_RXTXBIAS_CONFIG_CORE0		0x04f
#define R2057_TXGM_TXRF_PUS_CORE0		0x050
#define R2057_TXGM_IDAC_BLEED_CORE0		0x051
#define R2057_TXGM_GAIN_CORE0			0x056
#define R2057_TXGM2G_PKDET_PUS_CORE0		0x057
#define R2057_PAD2G_PTATS_CORE0			0x058
#define R2057_PAD2G_IDACS_CORE0			0x059
#define R2057_PAD2G_BOOST_PU_CORE0		0x05a
#define R2057_PAD2G_CASCV_GAIN_CORE0		0x05b
#define R2057_TXMIX2G_TUNE_BOOST_PU_CORE0	0x05c
#define R2057_TXMIX2G_LODC_CORE0		0x05d
#define R2057_PAD2G_TUNE_PUS_CORE0		0x05e
#define R2057_IPA2G_GAIN_CORE0			0x05f
#define R2057_TSSI2G_SPARE1_CORE0		0x060
#define R2057_TSSI2G_SPARE2_CORE0		0x061
#define R2057_IPA2G_TUNEV_CASCV_PTAT_CORE0	0x062
#define R2057_IPA2G_IMAIN_CORE0			0x063
#define R2057_IPA2G_CASCONV_CORE0		0x064
#define R2057_IPA2G_CASCOFFV_CORE0		0x065
#define R2057_IPA2G_BIAS_FILTER_CORE0		0x066
#define R2057_TX5G_PKDET_CORE0			0x069
#define R2057_PGA_PTAT_TXGM5G_PU_CORE0		0x06a
#define R2057_PAD5G_PTATS1_CORE0		0x06b
#define R2057_PAD5G_CLASS_PTATS2_CORE0		0x06c
#define R2057_PGA_BOOSTPTAT_IMAIN_CORE0		0x06d
#define R2057_PAD5G_CASCV_IMAIN_CORE0		0x06e
#define R2057_TXMIX5G_IBOOST_PAD_IAUX_CORE0	0x06f
#define R2057_PGA_BOOST_TUNE_CORE0		0x070
#define R2057_PGA_GAIN_CORE0			0x071
#define R2057_PAD5G_CASCOFFV_GAIN_PUS_CORE0	0x072
#define R2057_TXMIX5G_BOOST_TUNE_CORE0		0x073
#define R2057_PAD5G_TUNE_MISC_PUS_CORE0		0x074
#define R2057_IPA5G_IAUX_CORE0			0x075
#define R2057_IPA5G_GAIN_CORE0			0x076
#define R2057_TSSI5G_SPARE1_CORE0		0x077
#define R2057_TSSI5G_SPARE2_CORE0		0x078
#define R2057_IPA5G_CASCOFFV_PU_CORE0		0x079
#define R2057_IPA5G_PTAT_CORE0			0x07a
#define R2057_IPA5G_IMAIN_CORE0			0x07b
#define R2057_IPA5G_CASCONV_CORE0		0x07c
#define R2057_IPA5G_BIAS_FILTER_CORE0		0x07d
#define R2057_PAD_BIAS_FILTER_BWS_CORE0		0x080
#define R2057_TR2G_CONFIG1_CORE0_NU		0x081
#define R2057_TR2G_CONFIG2_CORE0_NU		0x082
#define R2057_LNA5G_RFEN_CORE0			0x083
#define R2057_TR5G_CONFIG2_CORE0_NU		0x084
#define R2057_RXRFBIAS_IBOOST_PU_CORE0		0x085
#define R2057_RXRF_IABAND_RXGM_IMAIN_PTAT_CORE0	0x086
#define R2057_RXGM_CMFBITAIL_AUXPTAT_CORE0	0x087
#define R2057_RXMIX_ICORE_RXGM_IAUX_CORE0	0x088
#define R2057_RXMIX_CMFBITAIL_PU_CORE0		0x089
#define R2057_LNA2_IMAIN_PTAT_PU_CORE0		0x08a
#define R2057_LNA2_IAUX_PTAT_CORE0		0x08b
#define R2057_LNA1_IMAIN_PTAT_PU_CORE0		0x08c
#define R2057_LNA15G_INPUT_MATCH_TUNE_CORE0	0x08d
#define R2057_RXRFBIAS_BANDSEL_CORE0		0x08e
#define R2057_TIA_CONFIG_CORE0			0x08f
#define R2057_TIA_IQGAIN_CORE0			0x090
#define R2057_TIA_IBIAS2_CORE0			0x091
#define R2057_TIA_IBIAS1_CORE0			0x092
#define R2057_TIA_SPARE_Q_CORE0			0x093
#define R2057_TIA_SPARE_I_CORE0			0x094
#define R2057_RXMIX2G_PUS_CORE0			0x095
#define R2057_RXMIX2G_VCMREFS_CORE0		0x096
#define R2057_RXMIX2G_LODC_QI_CORE0		0x097
#define R2057_W12G_BW_LNA2G_PUS_CORE0		0x098
#define R2057_LNA2G_GAIN_CORE0			0x099
#define R2057_LNA2G_TUNE_CORE0			0x09a
#define R2057_RXMIX5G_PUS_CORE0			0x09b
#define R2057_RXMIX5G_VCMREFS_CORE0		0x09c
#define R2057_RXMIX5G_LODC_QI_CORE0		0x09d
#define R2057_W15G_BW_LNA5G_PUS_CORE0		0x09e
#define R2057_LNA5G_GAIN_CORE0			0x09f
#define R2057_LNA5G_TUNE_CORE0			0x0a0
#define R2057_LPFSEL_TXRX_RXBB_PUS_CORE0	0x0a1
#define R2057_RXBB_BIAS_MASTER_CORE0		0x0a2
#define R2057_RXBB_VGABUF_IDACS_CORE0		0x0a3
#define R2057_LPF_VCMREF_TXBUF_VCMREF_CORE0	0x0a4
#define R2057_TXBUF_VINCM_CORE0			0x0a5
#define R2057_TXBUF_IDACS_CORE0			0x0a6
#define R2057_LPF_RESP_RXBUF_BW_CORE0		0x0a7
#define R2057_RXBB_CC_CORE0			0x0a8
#define R2057_RXBB_SPARE3_CORE0			0x0a9
#define R2057_RXBB_RCCAL_HPC_CORE0		0x0aa
#define R2057_LPF_IDACS_CORE0			0x0ab
#define R2057_LPFBYP_DCLOOP_BYP_IDAC_CORE0	0x0ac
#define R2057_TXBUF_GAIN_CORE0			0x0ad
#define R2057_AFELOOPBACK_AACI_RESP_CORE0	0x0ae
#define R2057_RXBUF_DEGEN_CORE0			0x0af
#define R2057_RXBB_SPARE2_CORE0			0x0b0
#define R2057_RXBB_SPARE1_CORE0			0x0b1
#define R2057_RSSI_MASTER_CORE0			0x0b2
#define R2057_W2_MASTER_CORE0			0x0b3
#define R2057_NB_MASTER_CORE0			0x0b4
#define R2057_W2_IDACS0_Q_CORE0			0x0b5
#define R2057_W2_IDACS1_Q_CORE0			0x0b6
#define R2057_W2_IDACS0_I_CORE0			0x0b7
#define R2057_W2_IDACS1_I_CORE0			0x0b8
#define R2057_RSSI_GPAIOSEL_W1_IDACS_CORE0	0x0b9
#define R2057_NB_IDACS_Q_CORE0			0x0ba
#define R2057_NB_IDACS_I_CORE0			0x0bb
#define R2057_BACKUP4_CORE0			0x0c1
#define R2057_BACKUP3_CORE0			0x0c2
#define R2057_BACKUP2_CORE0			0x0c3
#define R2057_BACKUP1_CORE0			0x0c4
#define R2057_SPARE16_CORE0			0x0c5
#define R2057_SPARE15_CORE0			0x0c6
#define R2057_SPARE14_CORE0			0x0c7
#define R2057_SPARE13_CORE0			0x0c8
#define R2057_SPARE12_CORE0			0x0c9
#define R2057_SPARE11_CORE0			0x0ca
#define R2057_TX2G_BIAS_RESETS_CORE0		0x0cb
#define R2057_TX5G_BIAS_RESETS_CORE0		0x0cc
#define R2057_IQTEST_SEL_PU			0x0cd
#define R2057_XTAL_CONFIG2			0x0ce
#define R2057_BUFS_MISC_LPFBW_CORE0		0x0cf
#define R2057_TXLPF_RCCAL_CORE0			0x0d0
#define R2057_RXBB_GPAIOSEL_RXLPF_RCCAL_CORE0	0x0d1
#define R2057_LPF_GAIN_CORE0			0x0d2
#define R2057_DACBUF_IDACS_BW_CORE0		0x0d3

/* MISC core 1 */
#define R2057_RXTXBIAS_CONFIG_CORE1		0x0d4
#define R2057_TXGM_TXRF_PUS_CORE1		0x0d5
#define R2057_TXGM_IDAC_BLEED_CORE1		0x0d6
#define R2057_TXGM_GAIN_CORE1			0x0db
#define R2057_TXGM2G_PKDET_PUS_CORE1		0x0dc
#define R2057_PAD2G_PTATS_CORE1			0x0dd
#define R2057_PAD2G_IDACS_CORE1			0x0de
#define R2057_PAD2G_BOOST_PU_CORE1		0x0df
#define R2057_PAD2G_CASCV_GAIN_CORE1		0x0e0
#define R2057_TXMIX2G_TUNE_BOOST_PU_CORE1	0x0e1
#define R2057_TXMIX2G_LODC_CORE1		0x0e2
#define R2057_PAD2G_TUNE_PUS_CORE1		0x0e3
#define R2057_IPA2G_GAIN_CORE1			0x0e4
#define R2057_TSSI2G_SPARE1_CORE1		0x0e5
#define R2057_TSSI2G_SPARE2_CORE1		0x0e6
#define R2057_IPA2G_TUNEV_CASCV_PTAT_CORE1	0x0e7
#define R2057_IPA2G_IMAIN_CORE1			0x0e8
#define R2057_IPA2G_CASCONV_CORE1		0x0e9
#define R2057_IPA2G_CASCOFFV_CORE1		0x0ea
#define R2057_IPA2G_BIAS_FILTER_CORE1		0x0eb
#define R2057_TX5G_PKDET_CORE1			0x0ee
#define R2057_PGA_PTAT_TXGM5G_PU_CORE1		0x0ef
#define R2057_PAD5G_PTATS1_CORE1		0x0f0
#define R2057_PAD5G_CLASS_PTATS2_CORE1		0x0f1
#define R2057_PGA_BOOSTPTAT_IMAIN_CORE1		0x0f2
#define R2057_PAD5G_CASCV_IMAIN_CORE1		0x0f3
#define R2057_TXMIX5G_IBOOST_PAD_IAUX_CORE1	0x0f4
#define R2057_PGA_BOOST_TUNE_CORE1		0x0f5
#define R2057_PGA_GAIN_CORE1			0x0f6
#define R2057_PAD5G_CASCOFFV_GAIN_PUS_CORE1	0x0f7
#define R2057_TXMIX5G_BOOST_TUNE_CORE1		0x0f8
#define R2057_PAD5G_TUNE_MISC_PUS_CORE1		0x0f9
#define R2057_IPA5G_IAUX_CORE1			0x0fa
#define R2057_IPA5G_GAIN_CORE1			0x0fb
#define R2057_TSSI5G_SPARE1_CORE1		0x0fc
#define R2057_TSSI5G_SPARE2_CORE1		0x0fd
#define R2057_IPA5G_CASCOFFV_PU_CORE1		0x0fe
#define R2057_IPA5G_PTAT_CORE1			0x0ff
#define R2057_IPA5G_IMAIN_CORE1			0x100
#define R2057_IPA5G_CASCONV_CORE1		0x101
#define R2057_IPA5G_BIAS_FILTER_CORE1		0x102
#define R2057_PAD_BIAS_FILTER_BWS_CORE1		0x105
#define R2057_TR2G_CONFIG1_CORE1_NU		0x106
#define R2057_TR2G_CONFIG2_CORE1_NU		0x107
#define R2057_LNA5G_RFEN_CORE1			0x108
#define R2057_TR5G_CONFIG2_CORE1_NU		0x109
#define R2057_RXRFBIAS_IBOOST_PU_CORE1		0x10a
#define R2057_RXRF_IABAND_RXGM_IMAIN_PTAT_CORE1	0x10b
#define R2057_RXGM_CMFBITAIL_AUXPTAT_CORE1	0x10c
#define R2057_RXMIX_ICORE_RXGM_IAUX_CORE1	0x10d
#define R2057_RXMIX_CMFBITAIL_PU_CORE1		0x10e
#define R2057_LNA2_IMAIN_PTAT_PU_CORE1		0x10f
#define R2057_LNA2_IAUX_PTAT_CORE1		0x110
#define R2057_LNA1_IMAIN_PTAT_PU_CORE1		0x111
#define R2057_LNA15G_INPUT_MATCH_TUNE_CORE1	0x112
#define R2057_RXRFBIAS_BANDSEL_CORE1		0x113
#define R2057_TIA_CONFIG_CORE1			0x114
#define R2057_TIA_IQGAIN_CORE1			0x115
#define R2057_TIA_IBIAS2_CORE1			0x116
#define R2057_TIA_IBIAS1_CORE1			0x117
#define R2057_TIA_SPARE_Q_CORE1			0x118
#define R2057_TIA_SPARE_I_CORE1			0x119
#define R2057_RXMIX2G_PUS_CORE1			0x11a
#define R2057_RXMIX2G_VCMREFS_CORE1		0x11b
#define R2057_RXMIX2G_LODC_QI_CORE1		0x11c
#define R2057_W12G_BW_LNA2G_PUS_CORE1		0x11d
#define R2057_LNA2G_GAIN_CORE1			0x11e
#define R2057_LNA2G_TUNE_CORE1			0x11f
#define R2057_RXMIX5G_PUS_CORE1			0x120
#define R2057_RXMIX5G_VCMREFS_CORE1		0x121
#define R2057_RXMIX5G_LODC_QI_CORE1		0x122
#define R2057_W15G_BW_LNA5G_PUS_CORE1		0x123
#define R2057_LNA5G_GAIN_CORE1			0x124
#define R2057_LNA5G_TUNE_CORE1			0x125
#define R2057_LPFSEL_TXRX_RXBB_PUS_CORE1	0x126
#define R2057_RXBB_BIAS_MASTER_CORE1		0x127
#define R2057_RXBB_VGABUF_IDACS_CORE1		0x128
#define R2057_LPF_VCMREF_TXBUF_VCMREF_CORE1	0x129
#define R2057_TXBUF_VINCM_CORE1			0x12a
#define R2057_TXBUF_IDACS_CORE1			0x12b
#define R2057_LPF_RESP_RXBUF_BW_CORE1		0x12c
#define R2057_RXBB_CC_CORE1			0x12d
#define R2057_RXBB_SPARE3_CORE1			0x12e
#define R2057_RXBB_RCCAL_HPC_CORE1		0x12f
#define R2057_LPF_IDACS_CORE1			0x130
#define R2057_LPFBYP_DCLOOP_BYP_IDAC_CORE1	0x131
#define R2057_TXBUF_GAIN_CORE1			0x132
#define R2057_AFELOOPBACK_AACI_RESP_CORE1	0x133
#define R2057_RXBUF_DEGEN_CORE1			0x134
#define R2057_RXBB_SPARE2_CORE1			0x135
#define R2057_RXBB_SPARE1_CORE1			0x136
#define R2057_RSSI_MASTER_CORE1			0x137
#define R2057_W2_MASTER_CORE1			0x138
#define R2057_NB_MASTER_CORE1			0x139
#define R2057_W2_IDACS0_Q_CORE1			0x13a
#define R2057_W2_IDACS1_Q_CORE1			0x13b
#define R2057_W2_IDACS0_I_CORE1			0x13c
#define R2057_W2_IDACS1_I_CORE1			0x13d
#define R2057_RSSI_GPAIOSEL_W1_IDACS_CORE1	0x13e
#define R2057_NB_IDACS_Q_CORE1			0x13f
#define R2057_NB_IDACS_I_CORE1			0x140
#define R2057_BACKUP4_CORE1			0x146
#define R2057_BACKUP3_CORE1			0x147
#define R2057_BACKUP2_CORE1			0x148
#define R2057_BACKUP1_CORE1			0x149
#define R2057_SPARE16_CORE1			0x14a
#define R2057_SPARE15_CORE1			0x14b
#define R2057_SPARE14_CORE1			0x14c
#define R2057_SPARE13_CORE1			0x14d
#define R2057_SPARE12_CORE1			0x14e
#define R2057_SPARE11_CORE1			0x14f
#define R2057_TX2G_BIAS_RESETS_CORE1		0x150
#define R2057_TX5G_BIAS_RESETS_CORE1		0x151
#define R2057_SPARE8_CORE1			0x152
#define R2057_SPARE7_CORE1			0x153
#define R2057_BUFS_MISC_LPFBW_CORE1		0x154
#define R2057_TXLPF_RCCAL_CORE1			0x155
#define R2057_RXBB_GPAIOSEL_RXLPF_RCCAL_CORE1	0x156
#define R2057_LPF_GAIN_CORE1			0x157
#define R2057_DACBUF_IDACS_BW_CORE1		0x158

#define R2057_DACBUF_VINCM_CORE1		0x159
#define R2057_RCCAL_START_R1_Q1_P1		0x15a
#define R2057_RCCAL_X1				0x15b
#define R2057_RCCAL_TRC0			0x15c
#define R2057_RCCAL_TRC1			0x15d
#define R2057_RCCAL_DONE_OSCCAP			0x15e
#define R2057_RCCAL_N0_0			0x15f
#define R2057_RCCAL_N0_1			0x160
#define R2057_RCCAL_N1_0			0x161
#define R2057_RCCAL_N1_1			0x162
#define R2057_RCAL_STATUS			0x163
#define R2057_XTALPUOVR_PINCTRL			0x164
#define R2057_OVR_REG0				0x165
#define R2057_OVR_REG1				0x166
#define R2057_OVR_REG2				0x167
#define R2057_OVR_REG3				0x168
#define R2057_OVR_REG4				0x169
#define R2057_RCCAL_SCAP_VAL			0x16a
#define R2057_RCCAL_BCAP_VAL			0x16b
#define R2057_RCCAL_HPC_VAL			0x16c
#define R2057_RCCAL_OVERRIDES			0x16d

/* TX core 0 */
#define R2057_TX0_IQCAL_GAIN_BW			0x170
#define R2057_TX0_LOFT_FINE_I			0x171
#define R2057_TX0_LOFT_FINE_Q			0x172
#define R2057_TX0_LOFT_COARSE_I			0x173
#define R2057_TX0_LOFT_COARSE_Q			0x174
#define R2057_TX0_TX_SSI_MASTER			0x175
#define R2057_TX0_IQCAL_VCM_HG			0x176
#define R2057_TX0_IQCAL_IDAC			0x177
#define R2057_TX0_TSSI_VCM			0x178
#define R2057_TX0_TX_SSI_MUX			0x179
#define R2057_TX0_TSSIA				0x17a
#define R2057_TX0_TSSIG				0x17b
#define R2057_TX0_TSSI_MISC1			0x17c
#define R2057_TX0_TXRXCOUPLE_2G_ATTEN		0x17d
#define R2057_TX0_TXRXCOUPLE_2G_PWRUP		0x17e
#define R2057_TX0_TXRXCOUPLE_5G_ATTEN		0x17f
#define R2057_TX0_TXRXCOUPLE_5G_PWRUP		0x180

/* TX core 1 */
#define R2057_TX1_IQCAL_GAIN_BW			0x190
#define R2057_TX1_LOFT_FINE_I			0x191
#define R2057_TX1_LOFT_FINE_Q			0x192
#define R2057_TX1_LOFT_COARSE_I			0x193
#define R2057_TX1_LOFT_COARSE_Q			0x194
#define R2057_TX1_TX_SSI_MASTER			0x195
#define R2057_TX1_IQCAL_VCM_HG			0x196
#define R2057_TX1_IQCAL_IDAC			0x197
#define R2057_TX1_TSSI_VCM			0x198
#define R2057_TX1_TX_SSI_MUX			0x199
#define R2057_TX1_TSSIA				0x19a
#define R2057_TX1_TSSIG				0x19b
#define R2057_TX1_TSSI_MISC1			0x19c
#define R2057_TX1_TXRXCOUPLE_2G_ATTEN		0x19d
#define R2057_TX1_TXRXCOUPLE_2G_PWRUP		0x19e
#define R2057_TX1_TXRXCOUPLE_5G_ATTEN		0x19f
#define R2057_TX1_TXRXCOUPLE_5G_PWRUP		0x1a0

#define R2057_AFE_VCM_CAL_MASTER_CORE0		0x1a1
#define R2057_AFE_SET_VCM_I_CORE0		0x1a2
#define R2057_AFE_SET_VCM_Q_CORE0		0x1a3
#define R2057_AFE_STATUS_VCM_IQADC_CORE0	0x1a4
#define R2057_AFE_STATUS_VCM_I_CORE0		0x1a5
#define R2057_AFE_STATUS_VCM_Q_CORE0		0x1a6
#define R2057_AFE_VCM_CAL_MASTER_CORE1		0x1a7
#define R2057_AFE_SET_VCM_I_CORE1		0x1a8
#define R2057_AFE_SET_VCM_Q_CORE1		0x1a9
#define R2057_AFE_STATUS_VCM_IQADC_CORE1	0x1aa
#define R2057_AFE_STATUS_VCM_I_CORE1		0x1ab
#define R2057_AFE_STATUS_VCM_Q_CORE1		0x1ac

#define R2057v7_DACBUF_VINCM_CORE0		0x1ad
#define R2057v7_RCCAL_MASTER			0x1ae
#define R2057v7_TR2G_CONFIG3_CORE0_NU		0x1af
#define R2057v7_TR2G_CONFIG3_CORE1_NU		0x1b0
#define R2057v7_LOGEN_PUS1			0x1b1
#define R2057v7_OVR_REG5			0x1b2
#define R2057v7_OVR_REG6			0x1b3
#define R2057v7_OVR_REG7			0x1b4
#define R2057v7_OVR_REG8			0x1b5
#define R2057v7_OVR_REG9			0x1b6
#define R2057v7_OVR_REG10			0x1b7
#define R2057v7_OVR_REG11			0x1b8
#define R2057v7_OVR_REG12			0x1b9
#define R2057v7_OVR_REG13			0x1ba
#define R2057v7_OVR_REG14			0x1bb
#define R2057v7_OVR_REG15			0x1bc
#define R2057v7_OVR_REG16			0x1bd
#define R2057v7_OVR_REG1			0x1be
#define R2057v7_OVR_REG18			0x1bf
#define R2057v7_OVR_REG19			0x1c0
#define R2057v7_OVR_REG20			0x1c1
#define R2057v7_OVR_REG21			0x1c2
#define R2057v7_OVR_REG2			0x1c3
#define R2057v7_OVR_REG23			0x1c4
#define R2057v7_OVR_REG24			0x1c5
#define R2057v7_OVR_REG25			0x1c6
#define R2057v7_OVR_REG26			0x1c7
#define R2057v7_OVR_REG27			0x1c8
#define R2057v7_OVR_REG28			0x1c9
#define R2057v7_IQTEST_SEL_PU2			0x1ca

#define R2057_VCM_MASK				0x7

struct b43_nphy_chantabent_rev7 {
	/* The channel frequency in MHz */
	u16 freq;
	/* Radio regs values on channelswitch */
	u8 radio_vcocal_countval0;
	u8 radio_vcocal_countval1;
	u8 radio_rfpll_refmaster_sparextalsize;
	u8 radio_rfpll_loopfilter_r1;
	u8 radio_rfpll_loopfilter_c2;
	u8 radio_rfpll_loopfilter_c1;
	u8 radio_cp_kpd_idac;
	u8 radio_rfpll_mmd0;
	u8 radio_rfpll_mmd1;
	u8 radio_vcobuf_tune;
	u8 radio_logen_mx2g_tune;
	u8 radio_logen_mx5g_tune;
	u8 radio_logen_indbuf2g_tune;
	u8 radio_logen_indbuf5g_tune;
	u8 radio_txmix2g_tune_boost_pu_core0;
	u8 radio_pad2g_tune_pus_core0;
	u8 radio_pga_boost_tune_core0;
	u8 radio_txmix5g_boost_tune_core0;
	u8 radio_pad5g_tune_misc_pus_core0;
	u8 radio_lna2g_tune_core0;
	u8 radio_lna5g_tune_core0;
	u8 radio_txmix2g_tune_boost_pu_core1;
	u8 radio_pad2g_tune_pus_core1;
	u8 radio_pga_boost_tune_core1;
	u8 radio_txmix5g_boost_tune_core1;
	u8 radio_pad5g_tune_misc_pus_core1;
	u8 radio_lna2g_tune_core1;
	u8 radio_lna5g_tune_core1;
	/* PHY res values on channelswitch */
	struct b43_phy_n_sfo_cfg phy_regs;
};

struct b43_nphy_chantabent_rev7_2g {
	/* The channel frequency in MHz */
	u16 freq;
	/* Radio regs values on channelswitch */
	u8 radio_vcocal_countval0;
	u8 radio_vcocal_countval1;
	u8 radio_rfpll_refmaster_sparextalsize;
	u8 radio_rfpll_loopfilter_r1;
	u8 radio_rfpll_loopfilter_c2;
	u8 radio_rfpll_loopfilter_c1;
	u8 radio_cp_kpd_idac;
	u8 radio_rfpll_mmd0;
	u8 radio_rfpll_mmd1;
	u8 radio_vcobuf_tune;
	u8 radio_logen_mx2g_tune;
	u8 radio_logen_indbuf2g_tune;
	u8 radio_txmix2g_tune_boost_pu_core0;
	u8 radio_pad2g_tune_pus_core0;
	u8 radio_lna2g_tune_core0;
	u8 radio_txmix2g_tune_boost_pu_core1;
	u8 radio_pad2g_tune_pus_core1;
	u8 radio_lna2g_tune_core1;
	/* PHY regs values on channelswitch */
	struct b43_phy_n_sfo_cfg phy_regs;
};

void r2057_upload_inittabs(struct b43_wldev *dev);

void r2057_get_chantabent_rev7(struct b43_wldev *dev, u16 freq,
			       const struct b43_nphy_chantabent_rev7 **tabent_r7,
			       const struct b43_nphy_chantabent_rev7_2g **tabent_r7_2g);

#endif /* B43_RADIO_2057_H_ */
