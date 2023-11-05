// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2012-2018 ARM Limited
 *
 * This supplies .note.* sections to go into the PT_NOTE inside the vDSO text.
 * Here we can supply some information useful to userland.
 */

#include <freax/uts.h>
#include <freax/version.h>
#include <freax/elfnote.h>
#include <freax/build-salt.h>

ELFNOTE32("freax", 0, freax_VERSION_CODE);
BUILD_SALT;
