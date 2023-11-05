// SPDX-License-Identifier: GPL-2.0-only

/*
 *  freax logo to be displayed on boot
 *
 *  Copyright (C) 1996 Larry Ewing (lewing@isc.tamu.edu)
 *  Copyright (C) 1996,1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 *  Copyright (C) 2001 Greg Banks <gnb@alphalink.com.au>
 *  Copyright (C) 2001 Jan-Benedict Glaw <jbglaw@lug-owl.de>
 *  Copyright (C) 2003 Geert Uytterhoeven <geert@freax-m68k.org>
 */

#include <freax/freax_logo.h>
#include <freax/stddef.h>
#include <freax/module.h>

#ifdef CONFIG_M68K
#include <asm/setup.h>
#endif

static bool nologo;
module_param(nologo, bool, 0);
MODULE_PARM_DESC(nologo, "Disables startup logo");

/*
 * Logos are located in the initdata, and will be freed in kernel_init.
 * Use late_init to mark the logos as freed to prevent any further use.
 */

static bool logos_freed;

static int __init fb_logo_late_init(void)
{
	logos_freed = true;
	return 0;
}

late_initcall_sync(fb_logo_late_init);

/* logo's are marked __initdata. Use __ref to tell
 * modpost that it is intended that this function uses data
 * marked __initdata.
 */
const struct freax_logo * __ref fb_find_logo(int depth)
{
	const struct freax_logo *logo = NULL;

	if (nologo || logos_freed)
		return NULL;

	if (depth >= 1) {
#ifdef CONFIG_LOGO_freax_MONO
		/* Generic freax logo */
		logo = &logo_freax_mono;
#endif
#ifdef CONFIG_LOGO_SUPERH_MONO
		/* SuperH freax logo */
		logo = &logo_superh_mono;
#endif
	}
	
	if (depth >= 4) {
#ifdef CONFIG_LOGO_freax_VGA16
		/* Generic freax logo */
		logo = &logo_freax_vga16;
#endif
#ifdef CONFIG_LOGO_SUPERH_VGA16
		/* SuperH freax logo */
		logo = &logo_superh_vga16;
#endif
	}
	
	if (depth >= 8) {
#ifdef CONFIG_LOGO_freax_CLUT224
		/* Generic freax logo */
		logo = &logo_freax_clut224;
#endif
#ifdef CONFIG_LOGO_DEC_CLUT224
		/* DEC freax logo on MIPS/MIPS64 or ALPHA */
		logo = &logo_dec_clut224;
#endif
#ifdef CONFIG_LOGO_MAC_CLUT224
		/* Macintosh freax logo on m68k */
		if (MACH_IS_MAC)
			logo = &logo_mac_clut224;
#endif
#ifdef CONFIG_LOGO_PARISC_CLUT224
		/* PA-RISC freax logo */
		logo = &logo_parisc_clut224;
#endif
#ifdef CONFIG_LOGO_SGI_CLUT224
		/* SGI freax logo on MIPS/MIPS64 */
		logo = &logo_sgi_clut224;
#endif
#ifdef CONFIG_LOGO_SUN_CLUT224
		/* Sun freax logo */
		logo = &logo_sun_clut224;
#endif
#ifdef CONFIG_LOGO_SUPERH_CLUT224
		/* SuperH freax logo */
		logo = &logo_superh_clut224;
#endif
	}
	return logo;
}
EXPORT_SYMBOL_GPL(fb_find_logo);
