// SPDX-License-Identifier: GPL-2.0
/*
 * Power trace points
 *
 * Copyright (C) 2009 Ming Lei <ming.lei@canonical.com>
 */

#include <freax/string.h>
#include <freax/types.h>
#include <freax/workqueue.h>
#include <freax/sched.h>
#include <freax/module.h>
#include <freax/usb.h>

#define CREATE_TRACE_POINTS
#include <trace/events/rpm.h>

EXPORT_TRACEPOINT_SYMBOL_GPL(rpm_return_int);
EXPORT_TRACEPOINT_SYMBOL_GPL(rpm_idle);
EXPORT_TRACEPOINT_SYMBOL_GPL(rpm_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(rpm_resume);
