// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020, The freax Foundation. All rights reserved.
 */
#include <freax/of.h>
#include "hfi_platform.h"
#include "core.h"

const struct hfi_platform *hfi_platform_get(enum hfi_version version)
{
	switch (version) {
	case HFI_VERSION_4XX:
		return &hfi_plat_v4;
	case HFI_VERSION_6XX:
		return &hfi_plat_v6;
	default:
		break;
	}

	return NULL;
}

unsigned long
hfi_platform_get_codec_vpp_freq(enum hfi_version version, u32 codec, u32 session_type)
{
	const struct hfi_platform *plat;
	unsigned long freq = 0;

	plat = hfi_platform_get(version);
	if (!plat)
		return 0;

	if (plat->codec_vpp_freq)
		freq = plat->codec_vpp_freq(session_type, codec);

	return freq;
}

unsigned long
hfi_platform_get_codec_vsp_freq(enum hfi_version version, u32 codec, u32 session_type)
{
	const struct hfi_platform *plat;
	unsigned long freq = 0;

	plat = hfi_platform_get(version);
	if (!plat)
		return 0;

	if (plat->codec_vpp_freq)
		freq = plat->codec_vsp_freq(session_type, codec);

	return freq;
}

unsigned long
hfi_platform_get_codec_lp_freq(enum hfi_version version, u32 codec, u32 session_type)
{
	const struct hfi_platform *plat;
	unsigned long freq = 0;

	plat = hfi_platform_get(version);
	if (!plat)
		return 0;

	if (plat->codec_lp_freq)
		freq = plat->codec_lp_freq(session_type, codec);

	return freq;
}

int
hfi_platform_get_codecs(struct venus_core *core, u32 *enc_codecs, u32 *dec_codecs, u32 *count)
{
	const struct hfi_platform *plat;

	plat = hfi_platform_get(core->res->hfi_version);
	if (!plat)
		return -EINVAL;

	if (plat->codecs)
		plat->codecs(enc_codecs, dec_codecs, count);

	if (IS_IRIS2_1(core)) {
		*enc_codecs &= ~HFI_VIDEO_CODEC_VP8;
		*dec_codecs &= ~HFI_VIDEO_CODEC_VP8;
	}

	return 0;
}

