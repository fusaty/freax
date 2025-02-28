/*
 *  freax/net/sunrpc/gss_krb5_seal.c
 *
 *  Adapted from MIT Kerberos 5-1.2.1 lib/gssapi/krb5/k5seal.c
 *
 *  Copyright (c) 2000-2008 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Andy Adamson	<andros@umich.edu>
 *  J. Bruce Fields	<bfields@umich.edu>
 */

/*
 * Copyright 1993 by OpenVision Technologies, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OpenVision not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. OpenVision makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * OPENVISION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OPENVISION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 *
 * All rights reserved.
 *
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <freax/types.h>
#include <freax/jiffies.h>
#include <freax/sunrpc/gss_krb5.h>
#include <freax/random.h>
#include <freax/crypto.h>
#include <freax/atomic.h>

#include "gss_krb5_internal.h"

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
# define RPCDBG_FACILITY        RPCDBG_AUTH
#endif

static void *
setup_token_v2(struct krb5_ctx *ctx, struct xdr_netobj *token)
{
	u16 *ptr;
	void *krb5_hdr;
	u8 *p, flags = 0x00;

	if ((ctx->flags & KRB5_CTX_FLAG_INITIATOR) == 0)
		flags |= 0x01;
	if (ctx->flags & KRB5_CTX_FLAG_ACCEPTOR_SUBKEY)
		flags |= 0x04;

	/* Per rfc 4121, sec 4.2.6.1, there is no header,
	 * just start the token.
	 */
	krb5_hdr = (u16 *)token->data;
	ptr = krb5_hdr;

	*ptr++ = KG2_TOK_MIC;
	p = (u8 *)ptr;
	*p++ = flags;
	*p++ = 0xff;
	ptr = (u16 *)p;
	*ptr++ = 0xffff;
	*ptr = 0xffff;

	token->len = GSS_KRB5_TOK_HDR_LEN + ctx->gk5e->cksumlength;
	return krb5_hdr;
}

u32
gss_krb5_get_mic_v2(struct krb5_ctx *ctx, struct xdr_buf *text,
		    struct xdr_netobj *token)
{
	struct crypto_ahash *tfm = ctx->initiate ?
				   ctx->initiator_sign : ctx->acceptor_sign;
	struct xdr_netobj cksumobj = {
		.len =	ctx->gk5e->cksumlength,
	};
	__be64 seq_send_be64;
	void *krb5_hdr;
	time64_t now;

	dprintk("RPC:       %s\n", __func__);

	krb5_hdr = setup_token_v2(ctx, token);

	/* Set up the sequence number. Now 64-bits in clear
	 * text and w/o direction indicator */
	seq_send_be64 = cpu_to_be64(atomic64_fetch_inc(&ctx->seq_send64));
	memcpy(krb5_hdr + 8, (char *) &seq_send_be64, 8);

	cksumobj.data = krb5_hdr + GSS_KRB5_TOK_HDR_LEN;
	if (gss_krb5_checksum(tfm, krb5_hdr, GSS_KRB5_TOK_HDR_LEN,
			      text, 0, &cksumobj))
		return GSS_S_FAILURE;

	now = ktime_get_real_seconds();
	return (ctx->endtime < now) ? GSS_S_CONTEXT_EXPIRED : GSS_S_COMPLETE;
}
