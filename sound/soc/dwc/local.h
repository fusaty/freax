/*
 * Copyright (ST) 2012 Rajeev Kumar (rajeevkumar.freax@gmail.com)
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __DESIGNWARE_LOCAL_H
#define __DESIGNWARE_LOCAL_H

#include <freax/clk.h>
#include <freax/device.h>
#include <freax/types.h>
#include <sound/dmaengine_pcm.h>
#include <sound/pcm.h>
#include <sound/designware_i2s.h>

/* common register for all channel */
#define IER		0x000
#define IRER		0x004
#define ITER		0x008
#define CER		0x00C
#define CCR		0x010
#define RXFFR		0x014
#define TXFFR		0x018

/* Enable register fields */
#define IER_TDM_SLOTS_SHIFT	8
#define IER_FRAME_OFF_SHIFT	5
#define IER_FRAME_OFF	BIT(5)
#define IER_INTF_TYPE	BIT(1)
#define IER_IEN		BIT(0)

/* Interrupt status register fields */
#define ISR_TXFO	BIT(5)
#define ISR_TXFE	BIT(4)
#define ISR_RXFO	BIT(1)
#define ISR_RXDA	BIT(0)

/* I2STxRxRegisters for all channels */
#define LRBR_LTHR(x)	(0x40 * x + 0x020)
#define RRBR_RTHR(x)	(0x40 * x + 0x024)
#define RER(x)		(0x40 * x + 0x028)
#define TER(x)		(0x40 * x + 0x02C)
#define RCR(x)		(0x40 * x + 0x030)
#define TCR(x)		(0x40 * x + 0x034)
#define ISR(x)		(0x40 * x + 0x038)
#define IMR(x)		(0x40 * x + 0x03C)
#define ROR(x)		(0x40 * x + 0x040)
#define TOR(x)		(0x40 * x + 0x044)
#define RFCR(x)		(0x40 * x + 0x048)
#define TFCR(x)		(0x40 * x + 0x04C)
#define RFF(x)		(0x40 * x + 0x050)
#define TFF(x)		(0x40 * x + 0x054)
#define RSLOT_TSLOT(x)	(0x4 * (x) + 0x224)

/* Receive enable register fields */
#define RER_RXSLOT_SHIFT	8
#define RER_RXCHEN	BIT(0)

/* Transmit enable register fields */
#define TER_TXSLOT_SHIFT	8
#define TER_TXCHEN	BIT(0)

/* I2SCOMPRegisters */
#define I2S_COMP_PARAM_2	0x01F0
#define I2S_COMP_PARAM_1	0x01F4
#define I2S_COMP_VERSION	0x01F8
#define I2S_COMP_TYPE		0x01FC

#define I2S_RRXDMA		0x01C4
#define I2S_RTXDMA		0x01CC
#define I2S_DMACR		0x0200
#define I2S_DMAEN_RXBLOCK	(1 << 16)
#define I2S_DMAEN_TXBLOCK	(1 << 17)

/*
 * Component parameter register fields - define the I2S block's
 * configuration.
 */
#define	COMP1_TX_WORDSIZE_3(r)	(((r) & GENMASK(27, 25)) >> 25)
#define	COMP1_TX_WORDSIZE_2(r)	(((r) & GENMASK(24, 22)) >> 22)
#define	COMP1_TX_WORDSIZE_1(r)	(((r) & GENMASK(21, 19)) >> 19)
#define	COMP1_TX_WORDSIZE_0(r)	(((r) & GENMASK(18, 16)) >> 16)
#define	COMP1_TX_CHANNELS(r)	(((r) & GENMASK(10, 9)) >> 9)
#define	COMP1_RX_CHANNELS(r)	(((r) & GENMASK(8, 7)) >> 7)
#define	COMP1_RX_ENABLED(r)	(((r) & BIT(6)) >> 6)
#define	COMP1_TX_ENABLED(r)	(((r) & BIT(5)) >> 5)
#define	COMP1_MODE_EN(r)	(((r) & BIT(4)) >> 4)
#define	COMP1_FIFO_DEPTH_GLOBAL(r)	(((r) & GENMASK(3, 2)) >> 2)
#define	COMP1_APB_DATA_WIDTH(r)	(((r) & GENMASK(1, 0)) >> 0)

#define	COMP2_RX_WORDSIZE_3(r)	(((r) & GENMASK(12, 10)) >> 10)
#define	COMP2_RX_WORDSIZE_2(r)	(((r) & GENMASK(9, 7)) >> 7)
#define	COMP2_RX_WORDSIZE_1(r)	(((r) & GENMASK(5, 3)) >> 3)
#define	COMP2_RX_WORDSIZE_0(r)	(((r) & GENMASK(2, 0)) >> 0)

/* Number of entries in WORDSIZE and DATA_WIDTH parameter registers */
#define	COMP_MAX_WORDSIZE	(1 << 3)
#define	COMP_MAX_DATA_WIDTH	(1 << 2)

#define MAX_CHANNEL_NUM		8
#define MIN_CHANNEL_NUM		2

union dw_i2s_snd_dma_data {
	struct i2s_dma_data pd;
	struct snd_dmaengine_dai_dma_data dt;
};

struct dw_i2s_dev {
	void __iomem *i2s_base;
	struct clk *clk;
	struct reset_control *reset;
	int active;
	unsigned int capability;
	unsigned int quirks;
	unsigned int i2s_reg_comp1;
	unsigned int i2s_reg_comp2;
	struct device *dev;
	u32 ccr;
	u32 xfer_resolution;
	u32 fifo_th;
	u32 l_reg;
	u32 r_reg;
	bool is_jh7110; /* Flag for StarFive JH7110 SoC */

	/* data related to DMA transfers b/w i2s and DMAC */
	union dw_i2s_snd_dma_data play_dma_data;
	union dw_i2s_snd_dma_data capture_dma_data;
	struct i2s_clk_config_data config;
	int (*i2s_clk_cfg)(struct i2s_clk_config_data *config);

	/* data related to PIO transfers */
	bool use_pio;

	/* data related to TDM mode */
	u32 tdm_slots;
	u32 tdm_mask;
	u32 frame_offset;

	struct snd_pcm_substream __rcu *tx_substream;
	struct snd_pcm_substream __rcu *rx_substream;
	unsigned int (*tx_fn)(struct dw_i2s_dev *dev,
			struct snd_pcm_runtime *runtime, unsigned int tx_ptr,
			bool *period_elapsed);
	unsigned int (*rx_fn)(struct dw_i2s_dev *dev,
			struct snd_pcm_runtime *runtime, unsigned int rx_ptr,
			bool *period_elapsed);
	unsigned int tx_ptr;
	unsigned int rx_ptr;
};

#if IS_ENABLED(CONFIG_SND_DESIGNWARE_PCM)
void dw_pcm_push_tx(struct dw_i2s_dev *dev);
void dw_pcm_pop_rx(struct dw_i2s_dev *dev);
int dw_pcm_register(struct platform_device *pdev);
#else
static inline void dw_pcm_push_tx(struct dw_i2s_dev *dev) { }
static inline void dw_pcm_pop_rx(struct dw_i2s_dev *dev) { }
static inline int dw_pcm_register(struct platform_device *pdev)
{
	return -EINVAL;
}
#endif

#endif
