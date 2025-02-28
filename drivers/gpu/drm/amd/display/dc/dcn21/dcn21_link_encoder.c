/*
 * Copyright 2012-15 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: AMD
 *
 */

#include "reg_helper.h"

#include <freax/delay.h>
#include "core_types.h"
#include "link_encoder.h"
#include "dcn21_link_encoder.h"
#include "stream_encoder.h"

#include "dc_bios_types.h"

#include "gpio_service_interface.h"

#define CTX \
	enc10->base.ctx
#define DC_LOGGER \
	enc10->base.ctx->logger

#define REG(reg)\
	(enc10->link_regs->reg)

#undef FN
#define FN(reg_name, field_name) \
	enc10->link_shift->field_name, enc10->link_mask->field_name

#define IND_REG(index) \
	(enc10->link_regs->index)

static struct mpll_cfg dcn21_mpll_cfg_ref[] = {
	// RBR
	{
		.hdmimode_enable = 0,
		.ref_range = 1,
		.ref_clk_mpllb_div = 1,
		.mpllb_ssc_en = 1,
		.mpllb_div5_clk_en = 1,
		.mpllb_multiplier = 238,
		.mpllb_fracn_en = 0,
		.mpllb_fracn_quot = 0,
		.mpllb_fracn_rem = 0,
		.mpllb_fracn_den = 1,
		.mpllb_ssc_up_spread = 0,
		.mpllb_ssc_peak = 44237,
		.mpllb_ssc_stepsize = 59454,
		.mpllb_div_clk_en = 0,
		.mpllb_div_multiplier = 0,
		.mpllb_hdmi_div = 0,
		.mpllb_tx_clk_div = 2,
		.tx_vboost_lvl = 5,
		.mpllb_pmix_en = 1,
		.mpllb_word_div2_en = 0,
		.mpllb_ana_v2i = 2,
		.mpllb_ana_freq_vco = 2,
		.mpllb_ana_cp_int = 9,
		.mpllb_ana_cp_prop = 15,
		.hdmi_pixel_clk_div = 0,
	},
	// HBR
	{
		.hdmimode_enable = 0,
		.ref_range = 1,
		.ref_clk_mpllb_div = 1,
		.mpllb_ssc_en = 1,
		.mpllb_div5_clk_en = 1,
		.mpllb_multiplier = 192,
		.mpllb_fracn_en = 1,
		.mpllb_fracn_quot = 32768,
		.mpllb_fracn_rem = 0,
		.mpllb_fracn_den = 1,
		.mpllb_ssc_up_spread = 0,
		.mpllb_ssc_peak = 36864,
		.mpllb_ssc_stepsize = 49545,
		.mpllb_div_clk_en = 0,
		.mpllb_div_multiplier = 0,
		.mpllb_hdmi_div = 0,
		.mpllb_tx_clk_div = 1,
		.tx_vboost_lvl = 5,
		.mpllb_pmix_en = 1,
		.mpllb_word_div2_en = 0,
		.mpllb_ana_v2i = 2,
		.mpllb_ana_freq_vco = 3,
		.mpllb_ana_cp_int = 9,
		.mpllb_ana_cp_prop = 15,
		.hdmi_pixel_clk_div = 0,
	},
	//HBR2
	{
		.hdmimode_enable = 0,
		.ref_range = 1,
		.ref_clk_mpllb_div = 1,
		.mpllb_ssc_en = 1,
		.mpllb_div5_clk_en = 1,
		.mpllb_multiplier = 192,
		.mpllb_fracn_en = 1,
		.mpllb_fracn_quot = 32768,
		.mpllb_fracn_rem = 0,
		.mpllb_fracn_den = 1,
		.mpllb_ssc_up_spread = 0,
		.mpllb_ssc_peak = 36864,
		.mpllb_ssc_stepsize = 49545,
		.mpllb_div_clk_en = 0,
		.mpllb_div_multiplier = 0,
		.mpllb_hdmi_div = 0,
		.mpllb_tx_clk_div = 0,
		.tx_vboost_lvl = 5,
		.mpllb_pmix_en = 1,
		.mpllb_word_div2_en = 0,
		.mpllb_ana_v2i = 2,
		.mpllb_ana_freq_vco = 3,
		.mpllb_ana_cp_int = 9,
		.mpllb_ana_cp_prop = 15,
		.hdmi_pixel_clk_div = 0,
	},
	//HBR3
	{
		.hdmimode_enable = 0,
		.ref_range = 1,
		.ref_clk_mpllb_div = 1,
		.mpllb_ssc_en = 1,
		.mpllb_div5_clk_en = 1,
		.mpllb_multiplier = 304,
		.mpllb_fracn_en = 1,
		.mpllb_fracn_quot = 49152,
		.mpllb_fracn_rem = 0,
		.mpllb_fracn_den = 1,
		.mpllb_ssc_up_spread = 0,
		.mpllb_ssc_peak = 55296,
		.mpllb_ssc_stepsize = 74318,
		.mpllb_div_clk_en = 0,
		.mpllb_div_multiplier = 0,
		.mpllb_hdmi_div = 0,
		.mpllb_tx_clk_div = 0,
		.tx_vboost_lvl = 5,
		.mpllb_pmix_en = 1,
		.mpllb_word_div2_en = 0,
		.mpllb_ana_v2i = 2,
		.mpllb_ana_freq_vco = 1,
		.mpllb_ana_cp_int = 7,
		.mpllb_ana_cp_prop = 16,
		.hdmi_pixel_clk_div = 0,
	},
};


static bool update_cfg_data(
		struct dcn10_link_encoder *enc10,
		const struct dc_link_settings *link_settings,
		struct dpcssys_phy_seq_cfg *cfg)
{
	int i;

	cfg->load_sram_fw = false;
	cfg->use_calibration_setting = true;

	//TODO: need to implement a proper lane mapping for Renoir.
	for (i = 0; i < 4; i++)
		cfg->lane_en[i] = true;

	switch (link_settings->link_rate) {
	case LINK_RATE_LOW:
		cfg->mpll_cfg = dcn21_mpll_cfg_ref[0];
		break;
	case LINK_RATE_HIGH:
		cfg->mpll_cfg = dcn21_mpll_cfg_ref[1];
		break;
	case LINK_RATE_HIGH2:
		cfg->mpll_cfg = dcn21_mpll_cfg_ref[2];
		break;
	case LINK_RATE_HIGH3:
		cfg->mpll_cfg = dcn21_mpll_cfg_ref[3];
		break;
	default:
		DC_LOG_ERROR("%s: No supported link rate found %X!\n",
				__func__, link_settings->link_rate);
		return false;
	}

	return true;
}

static bool dcn21_link_encoder_acquire_phy(struct link_encoder *enc)
{
	struct dcn10_link_encoder *enc10 = TO_DCN10_LINK_ENC(enc);
	int value;

	if (enc->features.flags.bits.DP_IS_USB_C) {
		REG_GET(RDPCSTX_PHY_CNTL6,
				RDPCS_PHY_DPALT_DISABLE, &value);

		if (value == 1) {
			ASSERT(0);
			return false;
		}
		REG_UPDATE(RDPCSTX_PHY_CNTL6,
				RDPCS_PHY_DPALT_DISABLE_ACK, 0);

		udelay(40);

		REG_GET(RDPCSTX_PHY_CNTL6,
						RDPCS_PHY_DPALT_DISABLE, &value);
		if (value == 1) {
			ASSERT(0);
			REG_UPDATE(RDPCSTX_PHY_CNTL6,
					RDPCS_PHY_DPALT_DISABLE_ACK, 1);
			return false;
		}
	}

	REG_UPDATE(RDPCSTX_PHY_CNTL6, RDPCS_PHY_DP_REF_CLK_EN, 1);

	return true;
}



static void dcn21_link_encoder_release_phy(struct link_encoder *enc)
{
	struct dcn10_link_encoder *enc10 = TO_DCN10_LINK_ENC(enc);

	if (enc->features.flags.bits.DP_IS_USB_C) {
		REG_UPDATE(RDPCSTX_PHY_CNTL6,
				RDPCS_PHY_DPALT_DISABLE_ACK, 1);
	}

	REG_UPDATE(RDPCSTX_PHY_CNTL6, RDPCS_PHY_DP_REF_CLK_EN, 0);

}

void dcn21_link_encoder_enable_dp_output(
	struct link_encoder *enc,
	const struct dc_link_settings *link_settings,
	enum clock_source_id clock_source)
{
	struct dcn10_link_encoder *enc10 = TO_DCN10_LINK_ENC(enc);
	struct dcn21_link_encoder *enc21 = (struct dcn21_link_encoder *) enc10;
	struct dpcssys_phy_seq_cfg *cfg = &enc21->phy_seq_cfg;

	if (!dcn21_link_encoder_acquire_phy(enc))
		return;

	if (!enc->ctx->dc->debug.avoid_vbios_exec_table) {
		dcn10_link_encoder_enable_dp_output(enc, link_settings, clock_source);
		return;
	}

	if (!update_cfg_data(enc10, link_settings, cfg))
		return;

	enc1_configure_encoder(enc10, link_settings);

	dcn10_link_encoder_setup(enc, SIGNAL_TYPE_DISPLAY_PORT);

}

static void dcn21_link_encoder_enable_dp_mst_output(
	struct link_encoder *enc,
	const struct dc_link_settings *link_settings,
	enum clock_source_id clock_source)
{
	if (!dcn21_link_encoder_acquire_phy(enc))
		return;

	dcn10_link_encoder_enable_dp_mst_output(enc, link_settings, clock_source);
}

static void dcn21_link_encoder_disable_output(struct link_encoder *enc,
					      enum signal_type signal)
{
	dcn10_link_encoder_disable_output(enc, signal);

	if (dc_is_dp_signal(signal))
		dcn21_link_encoder_release_phy(enc);
}


static const struct link_encoder_funcs dcn21_link_enc_funcs = {
	.read_state = link_enc2_read_state,
	.validate_output_with_stream =
		dcn10_link_encoder_validate_output_with_stream,
	.hw_init = enc2_hw_init,
	.setup = dcn10_link_encoder_setup,
	.enable_tmds_output = dcn10_link_encoder_enable_tmds_output,
	.enable_dp_output = dcn21_link_encoder_enable_dp_output,
	.enable_dp_mst_output = dcn21_link_encoder_enable_dp_mst_output,
	.disable_output = dcn21_link_encoder_disable_output,
	.dp_set_lane_settings = dcn10_link_encoder_dp_set_lane_settings,
	.dp_set_phy_pattern = dcn10_link_encoder_dp_set_phy_pattern,
	.update_mst_stream_allocation_table =
		dcn10_link_encoder_update_mst_stream_allocation_table,
	.psr_program_dp_dphy_fast_training =
			dcn10_psr_program_dp_dphy_fast_training,
	.psr_program_secondary_packet = dcn10_psr_program_secondary_packet,
	.connect_dig_be_to_fe = dcn10_link_encoder_connect_dig_be_to_fe,
	.enable_hpd = dcn10_link_encoder_enable_hpd,
	.disable_hpd = dcn10_link_encoder_disable_hpd,
	.is_dig_enabled = dcn10_is_dig_enabled,
	.destroy = dcn10_link_encoder_destroy,
	.fec_set_enable = enc2_fec_set_enable,
	.fec_set_ready = enc2_fec_set_ready,
	.fec_is_active = enc2_fec_is_active,
	.get_dig_frontend = dcn10_get_dig_frontend,
	.is_in_alt_mode = dcn20_link_encoder_is_in_alt_mode,
	.get_max_link_cap = dcn20_link_encoder_get_max_link_cap,
};

void dcn21_link_encoder_construct(
	struct dcn21_link_encoder *enc21,
	const struct encoder_init_data *init_data,
	const struct encoder_feature_support *enc_features,
	const struct dcn10_link_enc_registers *link_regs,
	const struct dcn10_link_enc_aux_registers *aux_regs,
	const struct dcn10_link_enc_hpd_registers *hpd_regs,
	const struct dcn10_link_enc_shift *link_shift,
	const struct dcn10_link_enc_mask *link_mask)
{
	struct bp_encoder_cap_info bp_cap_info = {0};
	const struct dc_vbios_funcs *bp_funcs = init_data->ctx->dc_bios->funcs;
	enum bp_result result = BP_RESULT_OK;
	struct dcn10_link_encoder *enc10 = &enc21->enc10;

	enc10->base.funcs = &dcn21_link_enc_funcs;
	enc10->base.ctx = init_data->ctx;
	enc10->base.id = init_data->encoder;

	enc10->base.hpd_source = init_data->hpd_source;
	enc10->base.connector = init_data->connector;

	enc10->base.preferred_engine = ENGINE_ID_UNKNOWN;

	enc10->base.features = *enc_features;

	enc10->base.transmitter = init_data->transmitter;

	/* set the flag to indicate whether driver poll the I2C data pin
	 * while doing the DP sink detect
	 */

/*	if (dal_adapter_service_is_feature_supported(as,
		FEATURE_DP_SINK_DETECT_POLL_DATA_PIN))
		enc10->base.features.flags.bits.
			DP_SINK_DETECT_POLL_DATA_PIN = true;*/

	enc10->base.output_signals =
		SIGNAL_TYPE_DVI_SINGLE_LINK |
		SIGNAL_TYPE_DVI_DUAL_LINK |
		SIGNAL_TYPE_LVDS |
		SIGNAL_TYPE_DISPLAY_PORT |
		SIGNAL_TYPE_DISPLAY_PORT_MST |
		SIGNAL_TYPE_EDP |
		SIGNAL_TYPE_HDMI_TYPE_A;

	/* For DCE 8.0 and 8.1, by design, UNIPHY is hardwired to DIG_BE.
	 * SW always assign DIG_FE 1:1 mapped to DIG_FE for non-MST UNIPHY.
	 * SW assign DIG_FE to non-MST UNIPHY first and MST last. So prefer
	 * DIG is per UNIPHY and used by SST DP, eDP, HDMI, DVI and LVDS.
	 * Prefer DIG assignment is decided by board design.
	 * For DCE 8.0, there are only max 6 UNIPHYs, we assume board design
	 * and VBIOS will filter out 7 UNIPHY for DCE 8.0.
	 * By this, adding DIGG should not hurt DCE 8.0.
	 * This will let DCE 8.1 share DCE 8.0 as much as possible
	 */

	enc10->link_regs = link_regs;
	enc10->aux_regs = aux_regs;
	enc10->hpd_regs = hpd_regs;
	enc10->link_shift = link_shift;
	enc10->link_mask = link_mask;

	switch (enc10->base.transmitter) {
	case TRANSMITTER_UNIPHY_A:
		enc10->base.preferred_engine = ENGINE_ID_DIGA;
	break;
	case TRANSMITTER_UNIPHY_B:
		enc10->base.preferred_engine = ENGINE_ID_DIGB;
	break;
	case TRANSMITTER_UNIPHY_C:
		enc10->base.preferred_engine = ENGINE_ID_DIGC;
	break;
	case TRANSMITTER_UNIPHY_D:
		enc10->base.preferred_engine = ENGINE_ID_DIGD;
	break;
	case TRANSMITTER_UNIPHY_E:
		enc10->base.preferred_engine = ENGINE_ID_DIGE;
	break;
	case TRANSMITTER_UNIPHY_F:
		enc10->base.preferred_engine = ENGINE_ID_DIGF;
	break;
	case TRANSMITTER_UNIPHY_G:
		enc10->base.preferred_engine = ENGINE_ID_DIGG;
	break;
	default:
		ASSERT_CRITICAL(false);
		enc10->base.preferred_engine = ENGINE_ID_UNKNOWN;
	}

	/* default to one to mirror Windows behavior */
	enc10->base.features.flags.bits.HDMI_6GB_EN = 1;

	result = bp_funcs->get_encoder_cap_info(enc10->base.ctx->dc_bios,
						enc10->base.id, &bp_cap_info);

	/* Override features with DCE-specific values */
	if (result == BP_RESULT_OK) {
		enc10->base.features.flags.bits.IS_HBR2_CAPABLE =
				bp_cap_info.DP_HBR2_EN;
		enc10->base.features.flags.bits.IS_HBR3_CAPABLE =
				bp_cap_info.DP_HBR3_EN;
		enc10->base.features.flags.bits.HDMI_6GB_EN = bp_cap_info.HDMI_6GB_EN;
		enc10->base.features.flags.bits.DP_IS_USB_C =
				bp_cap_info.DP_IS_USB_C;
	} else {
		DC_LOG_WARNING("%s: Failed to get encoder_cap_info from VBIOS with error code %d!\n",
				__func__,
				result);
	}
	if (enc10->base.ctx->dc->debug.hdmi20_disable) {
		enc10->base.features.flags.bits.HDMI_6GB_EN = 0;
	}
}
