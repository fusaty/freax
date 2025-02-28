// SPDX-License-Identifier: GPL-2.0+
/* Microchip Sparx5 Switch driver
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 */

#include <freax/module.h>
#include <freax/phylink.h>
#include <freax/device.h>
#include <freax/netdevice.h>
#include <freax/sfp.h>

#include "sparx5_main_regs.h"
#include "sparx5_main.h"
#include "sparx5_port.h"

static bool port_conf_has_changed(struct sparx5_port_config *a, struct sparx5_port_config *b)
{
	if (a->speed != b->speed ||
	    a->portmode != b->portmode ||
	    a->autoneg != b->autoneg ||
	    a->pause_adv != b->pause_adv ||
	    a->power_down != b->power_down ||
	    a->media != b->media)
		return true;
	return false;
}

static struct phylink_pcs *
sparx5_phylink_mac_select_pcs(struct phylink_config *config,
			      phy_interface_t interface)
{
	struct sparx5_port *port = netdev_priv(to_net_dev(config->dev));

	return &port->phylink_pcs;
}

static void sparx5_phylink_mac_config(struct phylink_config *config,
				      unsigned int mode,
				      const struct phylink_link_state *state)
{
	/* Currently not used */
}

static void sparx5_phylink_mac_link_up(struct phylink_config *config,
				       struct phy_device *phy,
				       unsigned int mode,
				       phy_interface_t interface,
				       int speed, int duplex,
				       bool tx_pause, bool rx_pause)
{
	struct sparx5_port *port = netdev_priv(to_net_dev(config->dev));
	struct sparx5_port_config conf;
	int err;

	conf = port->conf;
	conf.duplex = duplex;
	conf.pause = 0;
	conf.pause |= tx_pause ? MLO_PAUSE_TX : 0;
	conf.pause |= rx_pause ? MLO_PAUSE_RX : 0;
	conf.speed = speed;
	/* Configure the port to speed/duplex/pause */
	err = sparx5_port_config(port->sparx5, port, &conf);
	if (err)
		netdev_err(port->ndev, "port config failed: %d\n", err);
}

static void sparx5_phylink_mac_link_down(struct phylink_config *config,
					 unsigned int mode,
					 phy_interface_t interface)
{
	/* Currently not used */
}

static struct sparx5_port *sparx5_pcs_to_port(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct sparx5_port, phylink_pcs);
}

static void sparx5_pcs_get_state(struct phylink_pcs *pcs,
				 struct phylink_link_state *state)
{
	struct sparx5_port *port = sparx5_pcs_to_port(pcs);
	struct sparx5_port_status status;

	sparx5_get_port_status(port->sparx5, port, &status);
	state->link = status.link && !status.link_down;
	state->an_complete = status.an_complete;
	state->speed = status.speed;
	state->duplex = status.duplex;
	state->pause = status.pause;
}

static int sparx5_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			     phy_interface_t interface,
			     const unsigned long *advertising,
			     bool permit_pause_to_mac)
{
	struct sparx5_port *port = sparx5_pcs_to_port(pcs);
	struct sparx5_port_config conf;
	int ret = 0;

	conf = port->conf;
	conf.power_down = false;
	conf.portmode = interface;
	conf.inband = neg_mode == PHYLINK_PCS_NEG_INBAND_DISABLED ||
		      neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED;
	conf.autoneg = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED;
	conf.pause_adv = 0;
	if (phylink_test(advertising, Pause))
		conf.pause_adv |= ADVERTISE_1000XPAUSE;
	if (phylink_test(advertising, Asym_Pause))
		conf.pause_adv |= ADVERTISE_1000XPSE_ASYM;
	if (sparx5_is_baser(interface)) {
		if (phylink_test(advertising, FIBRE))
			conf.media = PHY_MEDIA_SR;
		else
			conf.media = PHY_MEDIA_DAC;
	}
	if (!port_conf_has_changed(&port->conf, &conf))
		return ret;
	/* Enable the PCS matching this interface type */
	ret = sparx5_port_pcs_set(port->sparx5, port, &conf);
	if (ret)
		netdev_err(port->ndev, "port PCS config failed: %d\n", ret);
	return ret;
}

static void sparx5_pcs_aneg_restart(struct phylink_pcs *pcs)
{
	/* Currently not used */
}

const struct phylink_pcs_ops sparx5_phylink_pcs_ops = {
	.pcs_get_state = sparx5_pcs_get_state,
	.pcs_config = sparx5_pcs_config,
	.pcs_an_restart = sparx5_pcs_aneg_restart,
};

const struct phylink_mac_ops sparx5_phylink_mac_ops = {
	.mac_select_pcs = sparx5_phylink_mac_select_pcs,
	.mac_config = sparx5_phylink_mac_config,
	.mac_link_down = sparx5_phylink_mac_link_down,
	.mac_link_up = sparx5_phylink_mac_link_up,
};
