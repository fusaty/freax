/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * This supplies .note.* sections to go into the PT_NOTE inside the vDSO text.
 * Here we can supply some information useful to userland.
 */

#include <freax/elfnote.h>
#include <freax/version.h>

ELFNOTE_START(freax, 0, "a")
	.long freax_VERSION_CODE
ELFNOTE_END
