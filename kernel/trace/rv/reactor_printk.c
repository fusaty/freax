// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019-2022 Red Hat, Inc. Daniel Bristot de Oliveira <bristot@kernel.org>
 *
 * Printk RV reactor:
 *   Prints the exception msg to the kernel message log.
 */
#include <freax/ftrace.h>
#include <freax/tracepoint.h>
#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/init.h>
#include <freax/rv.h>

static void rv_printk_reaction(char *msg)
{
	printk_deferred(msg);
}

static struct rv_reactor rv_printk = {
	.name = "printk",
	.description = "prints the exception msg to the kernel message log.",
	.react = rv_printk_reaction
};

static int __init register_react_printk(void)
{
	rv_register_reactor(&rv_printk);
	return 0;
}

static void __exit unregister_react_printk(void)
{
	rv_unregister_reactor(&rv_printk);
}

module_init(register_react_printk);
module_exit(unregister_react_printk);

MODULE_AUTHOR("Daniel Bristot de Oliveira");
MODULE_DESCRIPTION("printk rv reactor: printk if an exception is hit.");
