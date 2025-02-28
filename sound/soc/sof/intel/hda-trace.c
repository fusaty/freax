// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
//
// This file is provided under a dual BSD/GPLv2 license.  When using or
// redistributing this file, you may do so under either license.
//
// Copyright(c) 2018 Intel Corporation. All rights reserved.
//
// Authors: Liam Girdwood <liam.r.girdwood@freax.intel.com>
//	    Ranjani Sridharan <ranjani.sridharan@freax.intel.com>
//	    Rander Wang <rander.wang@intel.com>
//          Keyon Jie <yang.jie@freax.intel.com>
//

/*
 * Hardware interface for generic Intel audio DSP HDA IP
 */

#include <sound/hdaudio_ext.h>
#include "../ops.h"
#include "hda.h"

static int hda_dsp_trace_prepare(struct snd_sof_dev *sdev, struct snd_dma_buffer *dmab)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	struct hdac_ext_stream *hext_stream = hda->dtrace_stream;
	struct hdac_stream *hstream = &hext_stream->hstream;
	int ret;

	hstream->period_bytes = 0;/* initialize period_bytes */
	hstream->bufsize = dmab->bytes;

	ret = hda_dsp_stream_hw_params(sdev, hext_stream, dmab, NULL);
	if (ret < 0)
		dev_err(sdev->dev, "error: hdac prepare failed: %d\n", ret);

	return ret;
}

int hda_dsp_trace_init(struct snd_sof_dev *sdev, struct snd_dma_buffer *dmab,
		       struct sof_ipc_dma_trace_params_ext *dtrace_params)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	int ret;

	hda->dtrace_stream = hda_dsp_stream_get(sdev, SNDRV_PCM_STREAM_CAPTURE,
						SOF_HDA_STREAM_DMI_L1_COMPATIBLE);

	if (!hda->dtrace_stream) {
		dev_err(sdev->dev,
			"error: no available capture stream for DMA trace\n");
		return -ENODEV;
	}

	dtrace_params->stream_tag = hda->dtrace_stream->hstream.stream_tag;

	/*
	 * initialize capture stream, set BDL address and return corresponding
	 * stream tag which will be sent to the firmware by IPC message.
	 */
	ret = hda_dsp_trace_prepare(sdev, dmab);
	if (ret < 0) {
		dev_err(sdev->dev, "error: hdac trace init failed: %d\n", ret);
		hda_dsp_stream_put(sdev, SNDRV_PCM_STREAM_CAPTURE,
				   dtrace_params->stream_tag);
		hda->dtrace_stream = NULL;
		dtrace_params->stream_tag = 0;
	}

	return ret;
}

int hda_dsp_trace_release(struct snd_sof_dev *sdev)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;
	struct hdac_stream *hstream;

	if (hda->dtrace_stream) {
		hstream = &hda->dtrace_stream->hstream;
		hda_dsp_stream_put(sdev,
				   SNDRV_PCM_STREAM_CAPTURE,
				   hstream->stream_tag);
		hda->dtrace_stream = NULL;
		return 0;
	}

	dev_dbg(sdev->dev, "DMA trace stream is not opened!\n");
	return -ENODEV;
}

int hda_dsp_trace_trigger(struct snd_sof_dev *sdev, int cmd)
{
	struct sof_intel_hda_dev *hda = sdev->pdata->hw_pdata;

	return hda_dsp_stream_trigger(sdev, hda->dtrace_stream, cmd);
}
