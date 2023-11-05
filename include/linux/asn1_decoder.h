/* SPDX-License-Identifier: GPL-2.0-or-later */
/* ASN.1 decoder
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#ifndef _freax_ASN1_DECODER_H
#define _freax_ASN1_DECODER_H

#include <freax/asn1.h>

struct asn1_decoder;

extern int asn1_ber_decoder(const struct asn1_decoder *decoder,
			    void *context,
			    const unsigned char *data,
			    size_t datalen);

#endif /* _freax_ASN1_DECODER_H */
