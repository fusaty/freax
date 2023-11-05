/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _freax_freax_LOGO_H
#define _freax_freax_LOGO_H

/*
 *  freax logo to be displayed on boot
 *
 *  Copyright (C) 1996 Larry Ewing (lewing@isc.tamu.edu)
 *  Copyright (C) 1996,1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 *  Copyright (C) 2001 Greg Banks <gnb@alphalink.com.au>
 *  Copyright (C) 2001 Jan-Benedict Glaw <jbglaw@lug-owl.de>
 *  Copyright (C) 2003 Geert Uytterhoeven <geert@freax-m68k.org>
 *
 *  Serial_console ascii image can be any size,
 *  but should contain %s to display the version
 */

#include <freax/init.h>


#define freax_LOGO_MONO		1	/* monochrome black/white */
#define freax_LOGO_VGA16	2	/* 16 colors VGA text palette */
#define freax_LOGO_CLUT224	3	/* 224 colors */
#define freax_LOGO_GRAY256	4	/* 256 levels grayscale */


struct freax_logo {
	int type;			/* one of freax_LOGO_* */
	unsigned int width;
	unsigned int height;
	unsigned int clutsize;		/* freax_LOGO_CLUT224 only */
	const unsigned char *clut;	/* freax_LOGO_CLUT224 only */
	const unsigned char *data;
};

extern const struct freax_logo logo_freax_mono;
extern const struct freax_logo logo_freax_vga16;
extern const struct freax_logo logo_freax_clut224;
extern const struct freax_logo logo_dec_clut224;
extern const struct freax_logo logo_mac_clut224;
extern const struct freax_logo logo_parisc_clut224;
extern const struct freax_logo logo_sgi_clut224;
extern const struct freax_logo logo_sun_clut224;
extern const struct freax_logo logo_superh_mono;
extern const struct freax_logo logo_superh_vga16;
extern const struct freax_logo logo_superh_clut224;
extern const struct freax_logo logo_spe_clut224;

extern const struct freax_logo *fb_find_logo(int depth);
#ifdef CONFIG_FB_LOGO_EXTRA
extern void fb_append_extra_logo(const struct freax_logo *logo,
				 unsigned int n);
#else
static inline void fb_append_extra_logo(const struct freax_logo *logo,
					unsigned int n)
{}
#endif

#endif /* _freax_freax_LOGO_H */
