/* SPDX-License-Identifier: GPL-2.0 */
/* freax/spi/ads7846.h */

struct ads7846_platform_data {
	u16	model;			/* 7843, 7845, 7846, 7873. */
	u16	vref_delay_usecs;	/* 0 for external vref; etc */
	u16	vref_mv;		/* external vref value, milliVolts
					 * ads7846: if 0, use internal vref */
	bool	keep_vref_on;		/* set to keep vref on for differential
					 * measurements as well */
	bool	swap_xy;		/* swap x and y axes */

	/* Settling time of the analog signals; a function of Vcc and the
	 * capacitance on the X/Y drivers.  If set to non-zero, two samples
	 * are taken with settle_delay us apart, and the second one is used.
	 * ~150 uSec with 0.01uF caps.
	 */
	u16	settle_delay_usecs;

	/* If set to non-zero, after samples are taken this delay is applied
	 * and penirq is rechecked, to help avoid false events.  This value
	 * is affected by the material used to build the touch layer.
	 */
	u16	penirq_recheck_delay_usecs;

	u16	x_plate_ohms;
	u16	y_plate_ohms;

	u16	x_min, x_max;
	u16	y_min, y_max;
	u16	pressure_min, pressure_max;

	u16	debounce_max;		/* max number of additional readings
					 * per sample */
	u16	debounce_tol;		/* tolerance used for filtering */
	u16	debounce_rep;		/* additional consecutive good readings
					 * required after the first two */
	int	gpio_pendown_debounce;	/* platform specific debounce time for
					 * the gpio_pendown */
	int	(*get_pendown_state)(void);
	void	(*wait_for_sync)(void);
	bool	wakeup;
	unsigned long irq_flags;
};

