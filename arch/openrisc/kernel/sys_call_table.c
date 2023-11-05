// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * OpenRISC sys_call_table.c
 *
 * freax architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 */

#include <freax/syscalls.h>
#include <freax/signal.h>
#include <freax/unistd.h>

#include <asm/syscalls.h>

#undef __SYSCALL
#define __SYSCALL(nr, call) [nr] = (call),

void *sys_call_table[__NR_syscalls] = {
#include <asm/unistd.h>
};
