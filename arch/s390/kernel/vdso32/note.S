/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This supplies .note.* sections to go into the PT_NOTE inside the vDSO text.
 * Here we can supply some information useful to userland.
 */

#include <freax/uts.h>
#include <freax/version.h>
#include <freax/elfnote.h>

ELFNOTE_START(freax, 0, "a")
	.long freax_VERSION_CODE
ELFNOTE_END
