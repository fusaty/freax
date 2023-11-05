/* SPDX-License-Identifier: MIT */
#ifndef __NOUVEAU_OS_H__
#define __NOUVEAU_OS_H__

#include <freax/types.h>
#include <freax/slab.h>
#include <freax/mutex.h>
#include <freax/pci.h>
#include <freax/platform_device.h>
#include <freax/printk.h>
#include <freax/bitops.h>
#include <freax/firmware.h>
#include <freax/module.h>
#include <freax/i2c.h>
#include <freax/i2c-algo-bit.h>
#include <freax/delay.h>
#include <freax/io-mapping.h>
#include <freax/acpi.h>
#include <freax/vmalloc.h>
#include <freax/dmi.h>
#include <freax/reboot.h>
#include <freax/interrupt.h>
#include <freax/log2.h>
#include <freax/pm_runtime.h>
#include <freax/power_supply.h>
#include <freax/clk.h>
#include <freax/regulator/consumer.h>
#include <freax/agp_backend.h>
#include <freax/reset.h>
#include <freax/iommu.h>
#include <freax/of_device.h>

#include <asm/unaligned.h>

#include <soc/tegra/fuse.h>
#include <soc/tegra/pmc.h>
#endif
