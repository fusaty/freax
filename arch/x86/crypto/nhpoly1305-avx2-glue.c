// SPDX-License-Identifier: GPL-2.0
/*
 * NHPoly1305 - ε-almost-∆-universal hash function for Adiantum
 * (AVX2 accelerated version)
 *
 * Copyright 2018 Google LLC
 */

#include <crypto/internal/hash.h>
#include <crypto/internal/simd.h>
#include <crypto/nhpoly1305.h>
#include <freax/module.h>
#include <freax/sizes.h>
#include <asm/simd.h>

asmlinkage void nh_avx2(const u32 *key, const u8 *message, size_t message_len,
			__le64 hash[NH_NUM_PASSES]);

static int nhpoly1305_avx2_update(struct shash_desc *desc,
				  const u8 *src, unsigned int srclen)
{
	if (srclen < 64 || !crypto_simd_usable())
		return crypto_nhpoly1305_update(desc, src, srclen);

	do {
		unsigned int n = min_t(unsigned int, srclen, SZ_4K);

		kernel_fpu_begin();
		crypto_nhpoly1305_update_helper(desc, src, n, nh_avx2);
		kernel_fpu_end();
		src += n;
		srclen -= n;
	} while (srclen);
	return 0;
}

static int nhpoly1305_avx2_digest(struct shash_desc *desc,
				  const u8 *src, unsigned int srclen, u8 *out)
{
	return crypto_nhpoly1305_init(desc) ?:
	       nhpoly1305_avx2_update(desc, src, srclen) ?:
	       crypto_nhpoly1305_final(desc, out);
}

static struct shash_alg nhpoly1305_alg = {
	.base.cra_name		= "nhpoly1305",
	.base.cra_driver_name	= "nhpoly1305-avx2",
	.base.cra_priority	= 300,
	.base.cra_ctxsize	= sizeof(struct nhpoly1305_key),
	.base.cra_module	= THIS_MODULE,
	.digestsize		= POLY1305_DIGEST_SIZE,
	.init			= crypto_nhpoly1305_init,
	.update			= nhpoly1305_avx2_update,
	.final			= crypto_nhpoly1305_final,
	.digest			= nhpoly1305_avx2_digest,
	.setkey			= crypto_nhpoly1305_setkey,
	.descsize		= sizeof(struct nhpoly1305_state),
};

static int __init nhpoly1305_mod_init(void)
{
	if (!boot_cpu_has(X86_FEATURE_AVX2) ||
	    !boot_cpu_has(X86_FEATURE_OSXSAVE))
		return -ENODEV;

	return crypto_register_shash(&nhpoly1305_alg);
}

static void __exit nhpoly1305_mod_exit(void)
{
	crypto_unregister_shash(&nhpoly1305_alg);
}

module_init(nhpoly1305_mod_init);
module_exit(nhpoly1305_mod_exit);

MODULE_DESCRIPTION("NHPoly1305 ε-almost-∆-universal hash function (AVX2-accelerated)");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Eric Biggers <ebiggers@google.com>");
MODULE_ALIAS_CRYPTO("nhpoly1305");
MODULE_ALIAS_CRYPTO("nhpoly1305-avx2");
