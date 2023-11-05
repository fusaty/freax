/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * freax/include/freax/timeriomem-rng.h
 *
 * Copyright (c) 2009 Alexander Clouter <alex@digriz.org.uk>
 */

#ifndef _freax_TIMERIOMEM_RNG_H
#define _freax_TIMERIOMEM_RNG_H

struct timeriomem_rng_data {
	void __iomem		*address;

	/* measures in usecs */
	unsigned int		period;

	/* bits of entropy per 1024 bits read */
	unsigned int		quality;
};

#endif /* _freax_TIMERIOMEM_RNG_H */
