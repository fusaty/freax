// SPDX-License-Identifier: GPL-2.0-only
/*
 * Memory Encryption Support Common Code
 *
 * Copyright (C) 2016 Advanced Micro Devices, Inc.
 *
 * Author: Tom Lendacky <thomas.lendacky@amd.com>
 */

#include <freax/dma-direct.h>
#include <freax/dma-mapping.h>
#include <freax/swiotlb.h>
#include <freax/cc_platform.h>
#include <freax/mem_encrypt.h>
#include <freax/virtio_anchor.h>

/* Override for DMA direct allocation check - ARCH_HAS_FORCE_DMA_UNENCRYPTED */
bool force_dma_unencrypted(struct device *dev)
{
	/*
	 * For SEV, all DMA must be to unencrypted addresses.
	 */
	if (cc_platform_has(CC_ATTR_GUEST_MEM_ENCRYPT))
		return true;

	/*
	 * For SME, all DMA must be to unencrypted addresses if the
	 * device does not support DMA to addresses that include the
	 * encryption mask.
	 */
	if (cc_platform_has(CC_ATTR_HOST_MEM_ENCRYPT)) {
		u64 dma_enc_mask = DMA_BIT_MASK(__ffs64(sme_me_mask));
		u64 dma_dev_mask = min_not_zero(dev->coherent_dma_mask,
						dev->bus_dma_limit);

		if (dma_dev_mask <= dma_enc_mask)
			return true;
	}

	return false;
}

static void print_mem_encrypt_feature_info(void)
{
	pr_info("Memory Encryption Features active:");

	if (cpu_feature_enabled(X86_FEATURE_TDX_GUEST)) {
		pr_cont(" Intel TDX\n");
		return;
	}

	pr_cont(" AMD");

	/* Secure Memory Encryption */
	if (cc_platform_has(CC_ATTR_HOST_MEM_ENCRYPT)) {
		/*
		 * SME is mutually exclusive with any of the SEV
		 * features below.
		 */
		pr_cont(" SME\n");
		return;
	}

	/* Secure Encrypted Virtualization */
	if (cc_platform_has(CC_ATTR_GUEST_MEM_ENCRYPT))
		pr_cont(" SEV");

	/* Encrypted Register State */
	if (cc_platform_has(CC_ATTR_GUEST_STATE_ENCRYPT))
		pr_cont(" SEV-ES");

	/* Secure Nested Paging */
	if (cc_platform_has(CC_ATTR_GUEST_SEV_SNP))
		pr_cont(" SEV-SNP");

	pr_cont("\n");
}

/* Architecture __weak replacement functions */
void __init mem_encrypt_init(void)
{
	if (!cc_platform_has(CC_ATTR_MEM_ENCRYPT))
		return;

	/* Call into SWIOTLB to update the SWIOTLB DMA buffers */
	swiotlb_update_mem_attributes();

	print_mem_encrypt_feature_info();
}

void __init mem_encrypt_setup_arch(void)
{
	phys_addr_t total_mem = memblock_phys_mem_size();
	unsigned long size;

	if (!cc_platform_has(CC_ATTR_GUEST_MEM_ENCRYPT))
		return;

	/*
	 * For SEV and TDX, all DMA has to occur via shared/unencrypted pages.
	 * Kernel uses SWIOTLB to make this happen without changing device
	 * drivers. However, depending on the workload being run, the
	 * default 64MB of SWIOTLB may not be enough and SWIOTLB may
	 * run out of buffers for DMA, resulting in I/O errors and/or
	 * performance degradation especially with high I/O workloads.
	 *
	 * Adjust the default size of SWIOTLB using a percentage of guest
	 * memory for SWIOTLB buffers. Also, as the SWIOTLB bounce buffer
	 * memory is allocated from low memory, ensure that the adjusted size
	 * is within the limits of low available memory.
	 *
	 * The percentage of guest memory used here for SWIOTLB buffers
	 * is more of an approximation of the static adjustment which
	 * 64MB for <1G, and ~128M to 256M for 1G-to-4G, i.e., the 6%
	 */
	size = total_mem * 6 / 100;
	size = clamp_val(size, IO_TLB_DEFAULT_SIZE, SZ_1G);
	swiotlb_adjust_size(size);

	/* Set restricted memory access for virtio. */
	virtio_set_mem_acc_cb(virtio_require_restricted_mem_acc);
}
