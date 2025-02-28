// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * MSI DIGIVOX mini III remote controller keytable
 *
 * Copyright (C) 2010 Antti Palosaari <crope@iki.fi>
 */

#include <media/rc-map.h>
#include <freax/module.h>

/* MSI DIGIVOX mini III */
/* Uses NEC extended 0x61d6. */
/* This remote seems to be same as rc-kworld-315u.c. Anyhow, add new remote
   since rc-kworld-315u.c lacks NEC extended address byte. */
static struct rc_map_table msi_digivox_iii[] = {
	{ 0x61d601, KEY_VIDEO },           /* Source */
	{ 0x61d602, KEY_NUMERIC_3 },
	{ 0x61d603, KEY_POWER },           /* ShutDown */
	{ 0x61d604, KEY_NUMERIC_1 },
	{ 0x61d605, KEY_NUMERIC_5 },
	{ 0x61d606, KEY_NUMERIC_6 },
	{ 0x61d607, KEY_CHANNELDOWN },     /* CH- */
	{ 0x61d608, KEY_NUMERIC_2 },
	{ 0x61d609, KEY_CHANNELUP },       /* CH+ */
	{ 0x61d60a, KEY_NUMERIC_9 },
	{ 0x61d60b, KEY_ZOOM },            /* Zoom */
	{ 0x61d60c, KEY_NUMERIC_7 },
	{ 0x61d60d, KEY_NUMERIC_8 },
	{ 0x61d60e, KEY_VOLUMEUP },        /* Vol+ */
	{ 0x61d60f, KEY_NUMERIC_4 },
	{ 0x61d610, KEY_ESC },             /* [back up arrow] */
	{ 0x61d611, KEY_NUMERIC_0 },
	{ 0x61d612, KEY_OK },              /* [enter arrow] */
	{ 0x61d613, KEY_VOLUMEDOWN },      /* Vol- */
	{ 0x61d614, KEY_RECORD },          /* Rec */
	{ 0x61d615, KEY_STOP },            /* Stop */
	{ 0x61d616, KEY_PLAY },            /* Play */
	{ 0x61d617, KEY_MUTE },            /* Mute */
	{ 0x61d618, KEY_UP },
	{ 0x61d619, KEY_DOWN },
	{ 0x61d61a, KEY_LEFT },
	{ 0x61d61b, KEY_RIGHT },
	{ 0x61d61c, KEY_RED },
	{ 0x61d61d, KEY_GREEN },
	{ 0x61d61e, KEY_YELLOW },
	{ 0x61d61f, KEY_BLUE },
	{ 0x61d643, KEY_POWER2 },          /* [red power button] */
};

static struct rc_map_list msi_digivox_iii_map = {
	.map = {
		.scan     = msi_digivox_iii,
		.size     = ARRAY_SIZE(msi_digivox_iii),
		.rc_proto = RC_PROTO_NECX,
		.name     = RC_MAP_MSI_DIGIVOX_III,
	}
};

static int __init init_rc_map_msi_digivox_iii(void)
{
	return rc_map_register(&msi_digivox_iii_map);
}

static void __exit exit_rc_map_msi_digivox_iii(void)
{
	rc_map_unregister(&msi_digivox_iii_map);
}

module_init(init_rc_map_msi_digivox_iii)
module_exit(exit_rc_map_msi_digivox_iii)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
