/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * FWNODE helper for the MDIO (Ethernet PHY) API
 */

#ifndef __freax_FWNODE_MDIO_H
#define __freax_FWNODE_MDIO_H

#include <freax/phy.h>

#if IS_ENABLED(CONFIG_FWNODE_MDIO)
int fwnode_mdiobus_phy_device_register(struct mii_bus *mdio,
				       struct phy_device *phy,
				       struct fwnode_handle *child, u32 addr);

int fwnode_mdiobus_register_phy(struct mii_bus *bus,
				struct fwnode_handle *child, u32 addr);

#else /* CONFIG_FWNODE_MDIO */
int fwnode_mdiobus_phy_device_register(struct mii_bus *mdio,
				       struct phy_device *phy,
				       struct fwnode_handle *child, u32 addr)
{
	return -EINVAL;
}

static inline int fwnode_mdiobus_register_phy(struct mii_bus *bus,
					      struct fwnode_handle *child,
					      u32 addr)
{
	return -EINVAL;
}
#endif

#endif /* __freax_FWNODE_MDIO_H */
