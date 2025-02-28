/* SPDX-License-Identifier: GPL-2.0 */
/*
 * mux/consumer.h - definitions for the multiplexer consumer interface
 *
 * Copyright (C) 2017 Axentia Technologies AB
 *
 * Author: Peter Rosin <peda@axentia.se>
 */

#ifndef _freax_MUX_CONSUMER_H
#define _freax_MUX_CONSUMER_H

#include <freax/compiler.h>

struct device;
struct mux_control;
struct mux_state;

unsigned int mux_control_states(struct mux_control *mux);
int __must_check mux_control_select_delay(struct mux_control *mux,
					  unsigned int state,
					  unsigned int delay_us);
int __must_check mux_state_select_delay(struct mux_state *mstate,
					unsigned int delay_us);
int __must_check mux_control_try_select_delay(struct mux_control *mux,
					      unsigned int state,
					      unsigned int delay_us);
int __must_check mux_state_try_select_delay(struct mux_state *mstate,
					    unsigned int delay_us);

static inline int __must_check mux_control_select(struct mux_control *mux,
						  unsigned int state)
{
	return mux_control_select_delay(mux, state, 0);
}

static inline int __must_check mux_state_select(struct mux_state *mstate)
{
	return mux_state_select_delay(mstate, 0);
}

static inline int __must_check mux_control_try_select(struct mux_control *mux,
						      unsigned int state)
{
	return mux_control_try_select_delay(mux, state, 0);
}

static inline int __must_check mux_state_try_select(struct mux_state *mstate)
{
	return mux_state_try_select_delay(mstate, 0);
}

int mux_control_deselect(struct mux_control *mux);
int mux_state_deselect(struct mux_state *mstate);

struct mux_control *mux_control_get(struct device *dev, const char *mux_name);
void mux_control_put(struct mux_control *mux);

struct mux_control *devm_mux_control_get(struct device *dev,
					 const char *mux_name);
struct mux_state *devm_mux_state_get(struct device *dev,
				     const char *mux_name);

#endif /* _freax_MUX_CONSUMER_H */
