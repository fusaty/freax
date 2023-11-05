/****************************************************************************/

/*
 *	ucfreax.c -- generic memory mapped MTD driver for ucfreax
 *
 *	(C) Copyright 2002, Greg Ungerer (gerg@snapgear.com)
 *
 *      License: GPL
 */

/****************************************************************************/

#include <freax/moduleparam.h>
#include <freax/types.h>
#include <freax/init.h>
#include <freax/kernel.h>
#include <freax/fs.h>
#include <freax/mm.h>
#include <freax/major.h>
#include <freax/mtd/mtd.h>
#include <freax/mtd/map.h>
#include <freax/mtd/partitions.h>
#include <asm/io.h>
#include <asm/sections.h>

/****************************************************************************/

#ifdef CONFIG_MTD_ROM
#define MAP_NAME "rom"
#else
#define MAP_NAME "ram"
#endif

static struct map_info ucfreax_ram_map = {
	.name = MAP_NAME,
	.size = 0,
};

static unsigned long physaddr = -1;
module_param(physaddr, ulong, S_IRUGO);

static struct mtd_info *ucfreax_ram_mtdinfo;

/****************************************************************************/

static const struct mtd_partition ucfreax_romfs[] = {
	{ .name = "ROMfs" }
};

#define	NUM_PARTITIONS	ARRAY_SIZE(ucfreax_romfs)

/****************************************************************************/

static int ucfreax_point(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, void **virt, resource_size_t *phys)
{
	struct map_info *map = mtd->priv;
	*virt = map->virt + from;
	if (phys)
		*phys = map->phys + from;
	*retlen = len;
	return(0);
}

/****************************************************************************/

static int __init ucfreax_mtd_init(void)
{
	struct mtd_info *mtd;
	struct map_info *mapp;

	mapp = &ucfreax_ram_map;

	if (physaddr == -1)
		mapp->phys = (resource_size_t)__bss_stop;
	else
		mapp->phys = physaddr;

	if (!mapp->size)
		mapp->size = PAGE_ALIGN(ntohl(*((unsigned long *)(mapp->phys + 8))));
	mapp->bankwidth = 4;

	printk("ucfreax[mtd]: probe address=0x%x size=0x%x\n",
	       	(int) mapp->phys, (int) mapp->size);

	/*
	 * The filesystem is guaranteed to be in direct mapped memory. It is
	 * directly following the kernels own bss region. Following the same
	 * mechanism used by architectures setting up traditional initrds we
	 * use phys_to_virt to get the virtual address of its start.
	 */
	mapp->virt = phys_to_virt(mapp->phys);

	if (mapp->virt == 0) {
		printk("ucfreax[mtd]: no virtual mapping?\n");
		return(-EIO);
	}

	simple_map_init(mapp);

	mtd = do_map_probe("map_" MAP_NAME, mapp);
	if (!mtd) {
		printk("ucfreax[mtd]: failed to find a mapping?\n");
		return(-ENXIO);
	}

	mtd->owner = THIS_MODULE;
	mtd->_point = ucfreax_point;
	mtd->priv = mapp;

	ucfreax_ram_mtdinfo = mtd;
	mtd_device_register(mtd, ucfreax_romfs, NUM_PARTITIONS);

	return(0);
}
device_initcall(ucfreax_mtd_init);

/****************************************************************************/
