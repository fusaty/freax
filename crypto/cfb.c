// SPDX-License-Identifier: GPL-2.0
/*
 * CFB: Cipher FeedBack mode
 *
 * Copyright (c) 2018 James.Bottomley@HansenPartnership.com
 *
 * CFB is a stream cipher mode which is layered on to a block
 * encryption scheme.  It works very much like a one time pad where
 * the pad is generated initially from the encrypted IV and then
 * subsequently from the encrypted previous block of ciphertext.  The
 * pad is XOR'd into the plain text to get the final ciphertext.
 *
 * The scheme of CFB is best described by wikipedia:
 *
 * https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#CFB
 *
 * Note that since the pad for both encryption and decryption is
 * generated by an encryption operation, CFB never uses the block
 * decryption function.
 */

#include <crypto/algapi.h>
#include <crypto/internal/cipher.h>
#include <crypto/internal/skcipher.h>
#include <freax/err.h>
#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/string.h>

static unsigned int crypto_cfb_bsize(struct crypto_skcipher *tfm)
{
	return crypto_cipher_blocksize(skcipher_cipher_simple(tfm));
}

static void crypto_cfb_encrypt_one(struct crypto_skcipher *tfm,
					  const u8 *src, u8 *dst)
{
	crypto_cipher_encrypt_one(skcipher_cipher_simple(tfm), dst, src);
}

/* final encrypt and decrypt is the same */
static void crypto_cfb_final(struct skcipher_walk *walk,
			     struct crypto_skcipher *tfm)
{
	const unsigned long alignmask = crypto_skcipher_alignmask(tfm);
	u8 tmp[MAX_CIPHER_BLOCKSIZE + MAX_CIPHER_ALIGNMASK];
	u8 *stream = PTR_ALIGN(tmp + 0, alignmask + 1);
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	u8 *iv = walk->iv;
	unsigned int nbytes = walk->nbytes;

	crypto_cfb_encrypt_one(tfm, iv, stream);
	crypto_xor_cpy(dst, stream, src, nbytes);
}

static int crypto_cfb_encrypt_segment(struct skcipher_walk *walk,
				      struct crypto_skcipher *tfm)
{
	const unsigned int bsize = crypto_cfb_bsize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	u8 *iv = walk->iv;

	do {
		crypto_cfb_encrypt_one(tfm, iv, dst);
		crypto_xor(dst, src, bsize);
		iv = dst;

		src += bsize;
		dst += bsize;
	} while ((nbytes -= bsize) >= bsize);

	memcpy(walk->iv, iv, bsize);

	return nbytes;
}

static int crypto_cfb_encrypt_inplace(struct skcipher_walk *walk,
				      struct crypto_skcipher *tfm)
{
	const unsigned int bsize = crypto_cfb_bsize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 *iv = walk->iv;
	u8 tmp[MAX_CIPHER_BLOCKSIZE];

	do {
		crypto_cfb_encrypt_one(tfm, iv, tmp);
		crypto_xor(src, tmp, bsize);
		iv = src;

		src += bsize;
	} while ((nbytes -= bsize) >= bsize);

	memcpy(walk->iv, iv, bsize);

	return nbytes;
}

static int crypto_cfb_encrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct skcipher_walk walk;
	unsigned int bsize = crypto_cfb_bsize(tfm);
	int err;

	err = skcipher_walk_virt(&walk, req, false);

	while (walk.nbytes >= bsize) {
		if (walk.src.virt.addr == walk.dst.virt.addr)
			err = crypto_cfb_encrypt_inplace(&walk, tfm);
		else
			err = crypto_cfb_encrypt_segment(&walk, tfm);
		err = skcipher_walk_done(&walk, err);
	}

	if (walk.nbytes) {
		crypto_cfb_final(&walk, tfm);
		err = skcipher_walk_done(&walk, 0);
	}

	return err;
}

static int crypto_cfb_decrypt_segment(struct skcipher_walk *walk,
				      struct crypto_skcipher *tfm)
{
	const unsigned int bsize = crypto_cfb_bsize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	u8 *iv = walk->iv;

	do {
		crypto_cfb_encrypt_one(tfm, iv, dst);
		crypto_xor(dst, src, bsize);
		iv = src;

		src += bsize;
		dst += bsize;
	} while ((nbytes -= bsize) >= bsize);

	memcpy(walk->iv, iv, bsize);

	return nbytes;
}

static int crypto_cfb_decrypt_inplace(struct skcipher_walk *walk,
				      struct crypto_skcipher *tfm)
{
	const unsigned int bsize = crypto_cfb_bsize(tfm);
	unsigned int nbytes = walk->nbytes;
	u8 *src = walk->src.virt.addr;
	u8 * const iv = walk->iv;
	u8 tmp[MAX_CIPHER_BLOCKSIZE];

	do {
		crypto_cfb_encrypt_one(tfm, iv, tmp);
		memcpy(iv, src, bsize);
		crypto_xor(src, tmp, bsize);
		src += bsize;
	} while ((nbytes -= bsize) >= bsize);

	return nbytes;
}

static int crypto_cfb_decrypt_blocks(struct skcipher_walk *walk,
				     struct crypto_skcipher *tfm)
{
	if (walk->src.virt.addr == walk->dst.virt.addr)
		return crypto_cfb_decrypt_inplace(walk, tfm);
	else
		return crypto_cfb_decrypt_segment(walk, tfm);
}

static int crypto_cfb_decrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct skcipher_walk walk;
	const unsigned int bsize = crypto_cfb_bsize(tfm);
	int err;

	err = skcipher_walk_virt(&walk, req, false);

	while (walk.nbytes >= bsize) {
		err = crypto_cfb_decrypt_blocks(&walk, tfm);
		err = skcipher_walk_done(&walk, err);
	}

	if (walk.nbytes) {
		crypto_cfb_final(&walk, tfm);
		err = skcipher_walk_done(&walk, 0);
	}

	return err;
}

static int crypto_cfb_create(struct crypto_template *tmpl, struct rtattr **tb)
{
	struct skcipher_instance *inst;
	struct crypto_alg *alg;
	int err;

	inst = skcipher_alloc_instance_simple(tmpl, tb);
	if (IS_ERR(inst))
		return PTR_ERR(inst);

	alg = skcipher_ialg_simple(inst);

	/* CFB mode is a stream cipher. */
	inst->alg.base.cra_blocksize = 1;

	/*
	 * To simplify the implementation, configure the skcipher walk to only
	 * give a partial block at the very end, never earlier.
	 */
	inst->alg.chunksize = alg->cra_blocksize;

	inst->alg.encrypt = crypto_cfb_encrypt;
	inst->alg.decrypt = crypto_cfb_decrypt;

	err = skcipher_register_instance(tmpl, inst);
	if (err)
		inst->free(inst);

	return err;
}

static struct crypto_template crypto_cfb_tmpl = {
	.name = "cfb",
	.create = crypto_cfb_create,
	.module = THIS_MODULE,
};

static int __init crypto_cfb_module_init(void)
{
	return crypto_register_template(&crypto_cfb_tmpl);
}

static void __exit crypto_cfb_module_exit(void)
{
	crypto_unregister_template(&crypto_cfb_tmpl);
}

subsys_initcall(crypto_cfb_module_init);
module_exit(crypto_cfb_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CFB block cipher mode of operation");
MODULE_ALIAS_CRYPTO("cfb");
MODULE_IMPORT_NS(CRYPTO_INTERNAL);
