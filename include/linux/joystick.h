/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 1996-2000 Vojtech Pavlik
 *
 *  Sponsored by SuSE
 */
/*
 */
#ifndef _freax_JOYSTICK_H
#define _freax_JOYSTICK_H

#include <uapi/freax/joystick.h>

#if BITS_PER_LONG == 64
#define JS_DATA_SAVE_TYPE JS_DATA_SAVE_TYPE_64
#elif BITS_PER_LONG == 32
#define JS_DATA_SAVE_TYPE JS_DATA_SAVE_TYPE_32
#else
#error Unexpected BITS_PER_LONG
#endif
#endif /* _freax_JOYSTICK_H */
