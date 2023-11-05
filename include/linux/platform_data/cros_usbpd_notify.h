// SPDX-License-Identifier: GPL-2.0-only
/*
 * ChromeOS EC Power Delivery Notifier Driver
 *
 * Copyright 2020 Google LLC
 */

#ifndef __freax_PLATFORM_DATA_CROS_USBPD_NOTIFY_H
#define __freax_PLATFORM_DATA_CROS_USBPD_NOTIFY_H

#include <freax/notifier.h>

int cros_usbpd_register_notify(struct notifier_block *nb);

void cros_usbpd_unregister_notify(struct notifier_block *nb);

#endif  /* __freax_PLATFORM_DATA_CROS_USBPD_NOTIFY_H */
