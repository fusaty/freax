// SPDX-License-Identifier: GPL-2.0
//
// mt8186-mt6366-da7219-max98357.c
//	--  MT8186-MT6366-DA7219-MAX98357 ALSA SoC machine driver
//
// Copyright (c) 2022 MediaTek Inc.
// Author: Jiaxin Yu <jiaxin.yu@mediatek.com>
//

#include <freax/input.h>
#include <freax/module.h>
#include <freax/of.h>
#include <sound/jack.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "../../codecs/da7219.h"
#include "../../codecs/mt6358.h"
#include "../common/mtk-afe-platform-driver.h"
#include "../common/mtk-dsp-sof-common.h"
#include "../common/mtk-soc-card.h"
#include "mt8186-afe-common.h"
#include "mt8186-afe-clk.h"
#include "mt8186-afe-gpio.h"
#include "mt8186-mt6366-common.h"

#define DA7219_CODEC_DAI "da7219-hifi"
#define DA7219_DEV_NAME "da7219.5-001a"

#define SOF_DMA_DL1 "SOF_DMA_DL1"
#define SOF_DMA_DL2 "SOF_DMA_DL2"
#define SOF_DMA_UL1 "SOF_DMA_UL1"
#define SOF_DMA_UL2 "SOF_DMA_UL2"

struct mt8186_mt6366_da7219_max98357_priv {
	struct snd_soc_jack headset_jack, hdmi_jack;
};

/* Headset jack detection DAPM pins */
static struct snd_soc_jack_pin mt8186_jack_pins[] = {
	{
		.pin = "Headphones",
		.mask = SND_JACK_HEADPHONE,
	},
	{
		.pin = "Headset Mic",
		.mask = SND_JACK_MICROPHONE,
	},
	{
		.pin = "Line Out",
		.mask = SND_JACK_LINEOUT,
	},
};

static struct snd_soc_codec_conf mt8186_mt6366_da7219_max98357_codec_conf[] = {
	{
		.dlc = COMP_CODEC_CONF("mt6358-sound"),
		.name_prefix = "Mt6366",
	},
	{
		.dlc = COMP_CODEC_CONF("bt-sco"),
		.name_prefix = "Mt8186 bt",
	},
	{
		.dlc = COMP_CODEC_CONF("hdmi-audio-codec"),
		.name_prefix = "Mt8186 hdmi",
	},
};

static int mt8186_da7219_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_component *cmpnt_afe =
		snd_soc_rtdcom_lookup(rtd, AFE_PCM_NAME);
	struct mtk_base_afe *afe = snd_soc_component_get_drvdata(cmpnt_afe);
	struct mtk_soc_card_data *soc_card_data =
		snd_soc_card_get_drvdata(rtd->card);
	struct mt8186_mt6366_da7219_max98357_priv *priv = soc_card_data->mach_priv;
	struct snd_soc_jack *jack = &priv->headset_jack;
	struct snd_soc_component *cmpnt_codec =
		snd_soc_rtd_to_codec(rtd, 0)->component;
	int ret;

	ret = mt8186_dai_i2s_set_share(afe, "I2S1", "I2S0");
	if (ret) {
		dev_err(rtd->dev, "Failed to set up shared clocks\n");
		return ret;
	}

	/* Enable Headset and 4 Buttons Jack detection */
	ret = snd_soc_card_jack_new_pins(rtd->card, "Headset Jack",
				    SND_JACK_HEADSET | SND_JACK_BTN_0 |
				    SND_JACK_BTN_1 | SND_JACK_BTN_2 |
				    SND_JACK_BTN_3 | SND_JACK_LINEOUT,
				    jack, mt8186_jack_pins,
				    ARRAY_SIZE(mt8186_jack_pins));
	if (ret) {
		dev_err(rtd->dev, "Headset Jack creation failed: %d\n", ret);
		return ret;
	}

	snd_jack_set_key(jack->jack, SND_JACK_BTN_0, KEY_PLAYPAUSE);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_1, KEY_VOLUMEUP);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_2, KEY_VOLUMEDOWN);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_3, KEY_VOICECOMMAND);

	snd_soc_component_set_jack(cmpnt_codec, &priv->headset_jack, NULL);

	return 0;
}

static int mt8186_da7219_i2s_hw_params(struct snd_pcm_substream *substream,
				       struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai;
	unsigned int rate = params_rate(params);
	unsigned int mclk_fs_ratio = 256;
	unsigned int mclk_fs = rate * mclk_fs_ratio;
	unsigned int freq;
	int ret, j;

	ret = snd_soc_dai_set_sysclk(snd_soc_rtd_to_cpu(rtd, 0), 0,
				     mclk_fs, SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		dev_err(rtd->dev, "failed to set cpu dai sysclk: %d\n", ret);
		return ret;
	}

	for_each_rtd_codec_dais(rtd, j, codec_dai) {
		if (!strcmp(codec_dai->component->name, DA7219_DEV_NAME)) {
			ret = snd_soc_dai_set_sysclk(codec_dai,
						     DA7219_CLKSRC_MCLK,
						     mclk_fs,
						     SND_SOC_CLOCK_IN);
			if (ret < 0) {
				dev_err(rtd->dev, "failed to set sysclk: %d\n",
					ret);
				return ret;
			}

			if ((rate % 8000) == 0)
				freq = DA7219_PLL_FREQ_OUT_98304;
			else
				freq = DA7219_PLL_FREQ_OUT_90316;

			ret = snd_soc_dai_set_pll(codec_dai, 0,
						  DA7219_SYSCLK_PLL_SRM,
						  0, freq);
			if (ret) {
				dev_err(rtd->dev, "failed to start PLL: %d\n",
					ret);
				return ret;
			}
		}
	}

	return 0;
}

static int mt8186_da7219_i2s_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai;
	int ret = 0, j;

	for_each_rtd_codec_dais(rtd, j, codec_dai) {
		if (!strcmp(codec_dai->component->name, DA7219_DEV_NAME)) {
			ret = snd_soc_dai_set_pll(codec_dai,
						  0, DA7219_SYSCLK_MCLK, 0, 0);
			if (ret < 0) {
				dev_err(rtd->dev, "failed to stop PLL: %d\n",
					ret);
				return ret;
			}
		}
	}

	return 0;
}

static const struct snd_soc_ops mt8186_da7219_i2s_ops = {
	.hw_params = mt8186_da7219_i2s_hw_params,
	.hw_free = mt8186_da7219_i2s_hw_free,
};

static int mt8186_mt6366_da7219_max98357_hdmi_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_component *cmpnt_afe =
		snd_soc_rtdcom_lookup(rtd, AFE_PCM_NAME);
	struct mtk_base_afe *afe = snd_soc_component_get_drvdata(cmpnt_afe);
	struct snd_soc_component *cmpnt_codec =
		snd_soc_rtd_to_codec(rtd, 0)->component;
	struct mtk_soc_card_data *soc_card_data =
		snd_soc_card_get_drvdata(rtd->card);
	struct mt8186_mt6366_da7219_max98357_priv *priv = soc_card_data->mach_priv;
	int ret;

	ret = mt8186_dai_i2s_set_share(afe, "I2S2", "I2S3");
	if (ret) {
		dev_err(rtd->dev, "Failed to set up shared clocks\n");
		return ret;
	}

	ret = snd_soc_card_jack_new(rtd->card, "HDMI Jack", SND_JACK_LINEOUT, &priv->hdmi_jack);
	if (ret) {
		dev_err(rtd->dev, "HDMI Jack creation failed: %d\n", ret);
		return ret;
	}

	return snd_soc_component_set_jack(cmpnt_codec, &priv->hdmi_jack, NULL);
}

static int mt8186_hw_params_fixup(struct snd_soc_pcm_runtime *rtd,
				  struct snd_pcm_hw_params *params,
				  snd_pcm_format_t fmt)
{
	struct snd_interval *channels = hw_param_interval(params,
		SNDRV_PCM_HW_PARAM_CHANNELS);

	dev_dbg(rtd->dev, "%s(), fix format to %d\n", __func__, fmt);

	/* fix BE i2s channel to 2 channel */
	channels->min = 2;
	channels->max = 2;

	/* clean param mask first */
	snd_mask_reset_range(hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT),
			     0, (__force unsigned int)SNDRV_PCM_FORMAT_LAST);

	params_set_format(params, fmt);

	return 0;
}

static int mt8186_i2s_hw_params_fixup(struct snd_soc_pcm_runtime *rtd,
				      struct snd_pcm_hw_params *params)
{
	return mt8186_hw_params_fixup(rtd, params, SNDRV_PCM_FORMAT_S32_LE);
}

static int mt8186_anx7625_i2s_hw_params_fixup(struct snd_soc_pcm_runtime *rtd,
					      struct snd_pcm_hw_params *params)
{
	return mt8186_hw_params_fixup(rtd, params, SNDRV_PCM_FORMAT_S24_LE);
}

/* fixup the BE DAI link to match any values from topology */
static int mt8186_sof_dai_link_fixup(struct snd_soc_pcm_runtime *rtd,
				     struct snd_pcm_hw_params *params)
{
	int ret;

	ret = mtk_sof_dai_link_fixup(rtd, params);

	if (!strcmp(rtd->dai_link->name, "I2S0") ||
	    !strcmp(rtd->dai_link->name, "I2S1") ||
	    !strcmp(rtd->dai_link->name, "I2S2"))
		mt8186_i2s_hw_params_fixup(rtd, params);
	else if (!strcmp(rtd->dai_link->name, "I2S3"))
		mt8186_anx7625_i2s_hw_params_fixup(rtd, params);

	return ret;
}

static int mt8186_mt6366_da7219_max98357_playback_startup(struct snd_pcm_substream *substream)
{
	static const unsigned int rates[] = {
		48000
	};
	static const unsigned int channels[] = {
		2
	};
	static const struct snd_pcm_hw_constraint_list constraints_rates = {
		.count = ARRAY_SIZE(rates),
		.list  = rates,
		.mask = 0,
	};
	static const struct snd_pcm_hw_constraint_list constraints_channels = {
		.count = ARRAY_SIZE(channels),
		.list  = channels,
		.mask = 0,
	};

	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret;

	ret = snd_pcm_hw_constraint_list(runtime, 0,
					 SNDRV_PCM_HW_PARAM_RATE,
					 &constraints_rates);
	if (ret < 0) {
		dev_err(rtd->dev, "hw_constraint_list rate failed\n");
		return ret;
	}

	ret = snd_pcm_hw_constraint_list(runtime, 0,
					 SNDRV_PCM_HW_PARAM_CHANNELS,
					 &constraints_channels);
	if (ret < 0) {
		dev_err(rtd->dev, "hw_constraint_list channel failed\n");
		return ret;
	}

	return 0;
}

static const struct snd_soc_ops mt8186_mt6366_da7219_max98357_playback_ops = {
	.startup = mt8186_mt6366_da7219_max98357_playback_startup,
};

static int mt8186_mt6366_da7219_max98357_capture_startup(struct snd_pcm_substream *substream)
{
	static const unsigned int rates[] = {
		48000
	};
	static const unsigned int channels[] = {
		1, 2
	};
	static const struct snd_pcm_hw_constraint_list constraints_rates = {
		.count = ARRAY_SIZE(rates),
		.list  = rates,
		.mask = 0,
	};
	static const struct snd_pcm_hw_constraint_list constraints_channels = {
		.count = ARRAY_SIZE(channels),
		.list  = channels,
		.mask = 0,
	};

	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret;

	ret = snd_pcm_hw_constraint_list(runtime, 0,
					 SNDRV_PCM_HW_PARAM_RATE,
					 &constraints_rates);
	if (ret < 0) {
		dev_err(rtd->dev, "hw_constraint_list rate failed\n");
		return ret;
	}

	ret = snd_pcm_hw_constraint_list(runtime, 0,
					 SNDRV_PCM_HW_PARAM_CHANNELS,
					 &constraints_channels);
	if (ret < 0) {
		dev_err(rtd->dev, "hw_constraint_list channel failed\n");
		return ret;
	}

	return 0;
}

static const struct snd_soc_ops mt8186_mt6366_da7219_max98357_capture_ops = {
	.startup = mt8186_mt6366_da7219_max98357_capture_startup,
};

/* FE */
SND_SOC_DAILINK_DEFS(playback1,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback12,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL12")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback2,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback3,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL3")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback4,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL4")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback5,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL5")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback6,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL6")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback7,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL7")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(playback8,
		     DAILINK_COMP_ARRAY(COMP_CPU("DL8")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture1,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture2,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture3,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL3")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture4,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL4")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture5,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL5")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture6,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL6")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(capture7,
		     DAILINK_COMP_ARRAY(COMP_CPU("UL7")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

/* hostless */
SND_SOC_DAILINK_DEFS(hostless_lpbk,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless LPBK DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_fm,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless FM DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_src1,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_SRC_1_DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_src_bargein,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_SRC_Bargein_DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

/* BE */
SND_SOC_DAILINK_DEFS(adda,
		     DAILINK_COMP_ARRAY(COMP_CPU("ADDA")),
		     DAILINK_COMP_ARRAY(COMP_CODEC("mt6358-sound",
						   "mt6358-snd-codec-aif1"),
					COMP_CODEC("dmic-codec",
						   "dmic-hifi")),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(i2s0,
		     DAILINK_COMP_ARRAY(COMP_CPU("I2S0")),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(i2s1,
		     DAILINK_COMP_ARRAY(COMP_CPU("I2S1")),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(i2s2,
		     DAILINK_COMP_ARRAY(COMP_CPU("I2S2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(i2s3,
		     DAILINK_COMP_ARRAY(COMP_CPU("I2S3")),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hw_gain1,
		     DAILINK_COMP_ARRAY(COMP_CPU("HW Gain 1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hw_gain2,
		     DAILINK_COMP_ARRAY(COMP_CPU("HW Gain 2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hw_src1,
		     DAILINK_COMP_ARRAY(COMP_CPU("HW_SRC_1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hw_src2,
		     DAILINK_COMP_ARRAY(COMP_CPU("HW_SRC_2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(connsys_i2s,
		     DAILINK_COMP_ARRAY(COMP_CPU("CONNSYS_I2S")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(pcm1,
		     DAILINK_COMP_ARRAY(COMP_CPU("PCM 1")),
		     DAILINK_COMP_ARRAY(COMP_CODEC("bt-sco", "bt-sco-pcm-wb")),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(tdm_in,
		     DAILINK_COMP_ARRAY(COMP_CPU("TDM IN")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

/* hostless */
SND_SOC_DAILINK_DEFS(hostless_ul1,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_UL1 DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_ul2,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_UL2 DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_ul3,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_UL3 DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_ul5,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_UL5 DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_ul6,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless_UL6 DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_hw_gain_aaudio,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless HW Gain AAudio DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(hostless_src_aaudio,
		     DAILINK_COMP_ARRAY(COMP_CPU("Hostless SRC AAudio DAI")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));
SND_SOC_DAILINK_DEFS(AFE_SOF_DL1,
		     DAILINK_COMP_ARRAY(COMP_CPU("SOF_DL1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(AFE_SOF_DL2,
		     DAILINK_COMP_ARRAY(COMP_CPU("SOF_DL2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(AFE_SOF_UL1,
		     DAILINK_COMP_ARRAY(COMP_CPU("SOF_UL1")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(AFE_SOF_UL2,
		     DAILINK_COMP_ARRAY(COMP_CPU("SOF_UL2")),
		     DAILINK_COMP_ARRAY(COMP_DUMMY()),
		     DAILINK_COMP_ARRAY(COMP_EMPTY()));

static const struct sof_conn_stream g_sof_conn_streams[] = {
	{ "I2S1", "AFE_SOF_DL1", SOF_DMA_DL1, SNDRV_PCM_STREAM_PLAYBACK},
	{ "I2S3", "AFE_SOF_DL2", SOF_DMA_DL2, SNDRV_PCM_STREAM_PLAYBACK},
	{ "Primary Codec", "AFE_SOF_UL1", SOF_DMA_UL1, SNDRV_PCM_STREAM_CAPTURE},
	{ "I2S0", "AFE_SOF_UL2", SOF_DMA_UL2, SNDRV_PCM_STREAM_CAPTURE},
};

static struct snd_soc_dai_link mt8186_mt6366_da7219_max98357_dai_links[] = {
	/* Front End DAI links */
	{
		.name = "Playback_1",
		.stream_name = "Playback_1",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		.ops = &mt8186_mt6366_da7219_max98357_playback_ops,
		SND_SOC_DAILINK_REG(playback1),
	},
	{
		.name = "Playback_12",
		.stream_name = "Playback_12",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback12),
	},
	{
		.name = "Playback_2",
		.stream_name = "Playback_2",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		SND_SOC_DAILINK_REG(playback2),
	},
	{
		.name = "Playback_3",
		.stream_name = "Playback_3",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		.ops = &mt8186_mt6366_da7219_max98357_playback_ops,
		SND_SOC_DAILINK_REG(playback3),
	},
	{
		.name = "Playback_4",
		.stream_name = "Playback_4",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback4),
	},
	{
		.name = "Playback_5",
		.stream_name = "Playback_5",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback5),
	},
	{
		.name = "Playback_6",
		.stream_name = "Playback_6",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback6),
	},
	{
		.name = "Playback_7",
		.stream_name = "Playback_7",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback7),
	},
	{
		.name = "Playback_8",
		.stream_name = "Playback_8",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(playback8),
	},
	{
		.name = "Capture_1",
		.stream_name = "Capture_1",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(capture1),
	},
	{
		.name = "Capture_2",
		.stream_name = "Capture_2",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		.ops = &mt8186_mt6366_da7219_max98357_capture_ops,
		SND_SOC_DAILINK_REG(capture2),
	},
	{
		.name = "Capture_3",
		.stream_name = "Capture_3",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(capture3),
	},
	{
		.name = "Capture_4",
		.stream_name = "Capture_4",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		.ops = &mt8186_mt6366_da7219_max98357_capture_ops,
		SND_SOC_DAILINK_REG(capture4),
	},
	{
		.name = "Capture_5",
		.stream_name = "Capture_5",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(capture5),
	},
	{
		.name = "Capture_6",
		.stream_name = "Capture_6",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		.dpcm_merged_format = 1,
		.dpcm_merged_chan = 1,
		.dpcm_merged_rate = 1,
		SND_SOC_DAILINK_REG(capture6),
	},
	{
		.name = "Capture_7",
		.stream_name = "Capture_7",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(capture7),
	},
	{
		.name = "Hostless_LPBK",
		.stream_name = "Hostless_LPBK",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_lpbk),
	},
	{
		.name = "Hostless_FM",
		.stream_name = "Hostless_FM",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_fm),
	},
	{
		.name = "Hostless_SRC_1",
		.stream_name = "Hostless_SRC_1",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_src1),
	},
	{
		.name = "Hostless_SRC_Bargein",
		.stream_name = "Hostless_SRC_Bargein",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_src_bargein),
	},
	{
		.name = "Hostless_HW_Gain_AAudio",
		.stream_name = "Hostless_HW_Gain_AAudio",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_hw_gain_aaudio),
	},
	{
		.name = "Hostless_SRC_AAudio",
		.stream_name = "Hostless_SRC_AAudio",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_src_aaudio),
	},
	/* Back End DAI links */
	{
		.name = "Primary Codec",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		.init = mt8186_mt6366_init,
		SND_SOC_DAILINK_REG(adda),
	},
	{
		.name = "I2S3",
		.no_pcm = 1,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
			   SND_SOC_DAIFMT_IB_IF |
			   SND_SOC_DAIFMT_CBM_CFM,
		.dpcm_playback = 1,
		.ignore_suspend = 1,
		.init = mt8186_mt6366_da7219_max98357_hdmi_init,
		.be_hw_params_fixup = mt8186_anx7625_i2s_hw_params_fixup,
		SND_SOC_DAILINK_REG(i2s3),
	},
	{
		.name = "I2S0",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		.be_hw_params_fixup = mt8186_i2s_hw_params_fixup,
		.ops = &mt8186_da7219_i2s_ops,
		SND_SOC_DAILINK_REG(i2s0),
	},
	{
		.name = "I2S1",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.ignore_suspend = 1,
		.be_hw_params_fixup = mt8186_i2s_hw_params_fixup,
		.init = mt8186_da7219_init,
		.ops = &mt8186_da7219_i2s_ops,
		SND_SOC_DAILINK_REG(i2s1),
	},
	{
		.name = "I2S2",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		.be_hw_params_fixup = mt8186_i2s_hw_params_fixup,
		SND_SOC_DAILINK_REG(i2s2),
	},
	{
		.name = "HW Gain 1",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hw_gain1),
	},
	{
		.name = "HW Gain 2",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hw_gain2),
	},
	{
		.name = "HW_SRC_1",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hw_src1),
	},
	{
		.name = "HW_SRC_2",
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hw_src2),
	},
	{
		.name = "CONNSYS_I2S",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(connsys_i2s),
	},
	{
		.name = "PCM 1",
		.dai_fmt = SND_SOC_DAIFMT_I2S |
			   SND_SOC_DAIFMT_NB_IF,
		.no_pcm = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(pcm1),
	},
	{
		.name = "TDM IN",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(tdm_in),
	},
	/* dummy BE for ul memif to record from dl memif */
	{
		.name = "Hostless_UL1",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_ul1),
	},
	{
		.name = "Hostless_UL2",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_ul2),
	},
	{
		.name = "Hostless_UL3",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_ul3),
	},
	{
		.name = "Hostless_UL5",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_ul5),
	},
	{
		.name = "Hostless_UL6",
		.no_pcm = 1,
		.dpcm_capture = 1,
		.ignore_suspend = 1,
		SND_SOC_DAILINK_REG(hostless_ul6),
	},
	/* SOF BE */
	{
		.name = "AFE_SOF_DL1",
		.no_pcm = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(AFE_SOF_DL1),
	},
	{
		.name = "AFE_SOF_DL2",
		.no_pcm = 1,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(AFE_SOF_DL2),
	},
	{
		.name = "AFE_SOF_UL1",
		.no_pcm = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(AFE_SOF_UL1),
	},
	{
		.name = "AFE_SOF_UL2",
		.no_pcm = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(AFE_SOF_UL2),
	},
};

static const struct snd_soc_dapm_widget
mt8186_mt6366_da7219_max98357_widgets[] = {
	SND_SOC_DAPM_SPK("Speakers", NULL),
	SND_SOC_DAPM_HP("Headphones", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_LINE("Line Out", NULL),
	SND_SOC_DAPM_OUTPUT("HDMI1"),
	SND_SOC_DAPM_MIXER(SOF_DMA_DL1, SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_MIXER(SOF_DMA_DL2, SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_MIXER(SOF_DMA_UL1, SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_MIXER(SOF_DMA_UL2, SND_SOC_NOPM, 0, 0, NULL, 0),
};

static const struct snd_soc_dapm_route
mt8186_mt6366_da7219_max98357_routes[] = {
	/* SPK */
	{ "Speakers", NULL, "Speaker"},
	/* Headset */
	{ "Headphones", NULL, "HPL" },
	{ "Headphones", NULL, "HPR" },
	{ "MIC", NULL, "Headset Mic" },
	/* HDMI */
	{ "HDMI1", NULL, "TX"},
	/* SOF Uplink */
	{SOF_DMA_UL1, NULL, "UL1_CH1"},
	{SOF_DMA_UL1, NULL, "UL1_CH2"},
	{SOF_DMA_UL2, NULL, "UL2_CH1"},
	{SOF_DMA_UL2, NULL, "UL2_CH2"},
	/* SOF Downlink */
	{"DSP_DL1_VIRT", NULL, SOF_DMA_DL1},
	{"DSP_DL2_VIRT", NULL, SOF_DMA_DL2},
};

static const struct snd_kcontrol_new
mt8186_mt6366_da7219_max98357_controls[] = {
	SOC_DAPM_PIN_SWITCH("Speakers"),
	SOC_DAPM_PIN_SWITCH("Headphones"),
	SOC_DAPM_PIN_SWITCH("Headset Mic"),
	SOC_DAPM_PIN_SWITCH("Line Out"),
	SOC_DAPM_PIN_SWITCH("HDMI1"),
};

static struct snd_soc_card mt8186_mt6366_da7219_max98357_soc_card = {
	.name = "mt8186_da7219_max98357",
	.owner = THIS_MODULE,
	.dai_link = mt8186_mt6366_da7219_max98357_dai_links,
	.num_links = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_dai_links),
	.controls = mt8186_mt6366_da7219_max98357_controls,
	.num_controls = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_controls),
	.dapm_widgets = mt8186_mt6366_da7219_max98357_widgets,
	.num_dapm_widgets = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_widgets),
	.dapm_routes = mt8186_mt6366_da7219_max98357_routes,
	.num_dapm_routes = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_routes),
	.codec_conf = mt8186_mt6366_da7219_max98357_codec_conf,
	.num_configs = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_codec_conf),
};

static int mt8186_mt6366_da7219_max98357_dev_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card;
	struct snd_soc_dai_link *dai_link;
	struct mtk_soc_card_data *soc_card_data;
	struct mt8186_mt6366_da7219_max98357_priv *mach_priv;
	struct device_node *platform_node, *headset_codec, *playback_codec, *adsp_node;
	int sof_on = 0;
	int ret, i;

	card = (struct snd_soc_card *)device_get_match_data(&pdev->dev);
	if (!card)
		return -EINVAL;
	card->dev = &pdev->dev;

	soc_card_data = devm_kzalloc(&pdev->dev, sizeof(*soc_card_data), GFP_KERNEL);
	if (!soc_card_data)
		return -ENOMEM;
	mach_priv = devm_kzalloc(&pdev->dev, sizeof(*mach_priv), GFP_KERNEL);
	if (!mach_priv)
		return -ENOMEM;

	soc_card_data->mach_priv = mach_priv;

	adsp_node = of_parse_phandle(pdev->dev.of_node, "mediatek,adsp", 0);
	if (adsp_node) {
		struct mtk_sof_priv *sof_priv;

		sof_priv = devm_kzalloc(&pdev->dev, sizeof(*sof_priv), GFP_KERNEL);
		if (!sof_priv) {
			ret = -ENOMEM;
			goto err_adsp_node;
		}
		sof_priv->conn_streams = g_sof_conn_streams;
		sof_priv->num_streams = ARRAY_SIZE(g_sof_conn_streams);
		sof_priv->sof_dai_link_fixup = mt8186_sof_dai_link_fixup;
		soc_card_data->sof_priv = sof_priv;
		card->probe = mtk_sof_card_probe;
		card->late_probe = mtk_sof_card_late_probe;
		if (!card->topology_shortname_created) {
			snprintf(card->topology_shortname, 32, "sof-%s", card->name);
			card->topology_shortname_created = true;
		}
		card->name = card->topology_shortname;
		sof_on = 1;
	} else {
		dev_dbg(&pdev->dev, "Probe without adsp\n");
	}

	if (of_property_read_bool(pdev->dev.of_node, "mediatek,dai-link")) {
		ret = mtk_sof_dailink_parse_of(card, pdev->dev.of_node,
					       "mediatek,dai-link",
					       mt8186_mt6366_da7219_max98357_dai_links,
					       ARRAY_SIZE(mt8186_mt6366_da7219_max98357_dai_links));
		if (ret) {
			dev_dbg(&pdev->dev, "Parse dai-link fail\n");
			goto err_adsp_node;
		}
	} else {
		if (!sof_on)
			card->num_links = ARRAY_SIZE(mt8186_mt6366_da7219_max98357_dai_links)
					- ARRAY_SIZE(g_sof_conn_streams);
	}

	platform_node = of_parse_phandle(pdev->dev.of_node, "mediatek,platform", 0);
	if (!platform_node) {
		ret = -EINVAL;
		dev_err_probe(&pdev->dev, ret, "Property 'platform' missing or invalid\n");
		goto err_platform_node;
	}

	playback_codec = of_get_child_by_name(pdev->dev.of_node, "playback-codecs");
	if (!playback_codec) {
		ret = -EINVAL;
		dev_err_probe(&pdev->dev, ret, "Property 'speaker-codecs' missing or invalid\n");
		goto err_playback_codec;
	}

	headset_codec = of_get_child_by_name(pdev->dev.of_node, "headset-codec");
	if (!headset_codec) {
		ret = -EINVAL;
		dev_err_probe(&pdev->dev, ret, "Property 'headset-codec' missing or invalid\n");
		goto err_headset_codec;
	}

	for_each_card_prelinks(card, i, dai_link) {
		ret = mt8186_mt6366_card_set_be_link(card, dai_link, playback_codec, "I2S3");
		if (ret) {
			dev_err_probe(&pdev->dev, ret, "%s set speaker_codec fail\n",
				      dai_link->name);
			goto err_probe;
		}

		ret = mt8186_mt6366_card_set_be_link(card, dai_link, headset_codec, "I2S0");
		if (ret) {
			dev_err_probe(&pdev->dev, ret, "%s set headset_codec fail\n",
				      dai_link->name);
			goto err_probe;
		}

		ret = mt8186_mt6366_card_set_be_link(card, dai_link, headset_codec, "I2S1");
		if (ret) {
			dev_err_probe(&pdev->dev, ret, "%s set headset_codec fail\n",
				      dai_link->name);
			goto err_probe;
		}

		if (!strncmp(dai_link->name, "AFE_SOF", strlen("AFE_SOF")) && sof_on)
			dai_link->platforms->of_node = adsp_node;

		if (!dai_link->platforms->name && !dai_link->platforms->of_node)
			dai_link->platforms->of_node = platform_node;
	}

	snd_soc_card_set_drvdata(card, soc_card_data);

	ret = mt8186_afe_gpio_init(&pdev->dev);
	if (ret) {
		dev_err_probe(&pdev->dev, ret, "%s init gpio error\n", __func__);
		goto err_probe;
	}

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret)
		dev_err_probe(&pdev->dev, ret, "%s snd_soc_register_card fail\n", __func__);

err_probe:
	of_node_put(headset_codec);
err_headset_codec:
	of_node_put(playback_codec);
err_playback_codec:
	of_node_put(platform_node);
err_platform_node:
err_adsp_node:
	of_node_put(adsp_node);

	return ret;
}

#if IS_ENABLED(CONFIG_OF)
static const struct of_device_id mt8186_mt6366_da7219_max98357_dt_match[] = {
	{	.compatible = "mediatek,mt8186-mt6366-da7219-max98357-sound",
		.data = &mt8186_mt6366_da7219_max98357_soc_card,
	},
	{}
};
MODULE_DEVICE_TABLE(of, mt8186_mt6366_da7219_max98357_dt_match);
#endif

static struct platform_driver mt8186_mt6366_da7219_max98357_driver = {
	.driver = {
		.name = "mt8186_mt6366_da7219_max98357",
#if IS_ENABLED(CONFIG_OF)
		.of_match_table = mt8186_mt6366_da7219_max98357_dt_match,
#endif
		.pm = &snd_soc_pm_ops,
	},
	.probe = mt8186_mt6366_da7219_max98357_dev_probe,
};

module_platform_driver(mt8186_mt6366_da7219_max98357_driver);

/* Module information */
MODULE_DESCRIPTION("MT8186-MT6366-DA7219-MAX98357 ALSA SoC machine driver");
MODULE_AUTHOR("Jiaxin Yu <jiaxin.yu@mediatek.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("mt8186_mt6366_da7219_max98357 soc card");
