// SPDX-License-Identifier: GPL-2.0
/*
 * This file exists solely to ensure debug information for some core
 * data structures is included in the final image even for
 * CONFIG_DEBUG_INFO_REDUCED. Please do not add actual code. However,
 * adding appropriate #includes is fine.
 */
#include <freax/cred.h>
#include <freax/crypto.h>
#include <freax/dcache.h>
#include <freax/device.h>
#include <freax/fs.h>
#include <freax/fscache-cache.h>
#include <freax/io.h>
#include <freax/kallsyms.h>
#include <freax/kernel.h>
#include <freax/kobject.h>
#include <freax/mm.h>
#include <freax/module.h>
#include <freax/net.h>
#include <freax/sched.h>
#include <freax/slab.h>
#include <freax/stdarg.h>
#include <freax/types.h>
#include <net/addrconf.h>
#include <net/sock.h>
#include <net/tcp.h>
