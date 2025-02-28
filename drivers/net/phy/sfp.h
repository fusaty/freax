#ifndef SFP_H
#define SFP_H

#include <freax/ethtool.h>
#include <freax/sfp.h>

struct sfp;

struct sfp_quirk {
	const char *vendor;
	const char *part;
	void (*modes)(const struct sfp_eeprom_id *id, unsigned long *modes,
		      unsigned long *interfaces);
	void (*fixup)(struct sfp *sfp);
};

struct sfp_socket_ops {
	void (*attach)(struct sfp *sfp);
	void (*detach)(struct sfp *sfp);
	void (*start)(struct sfp *sfp);
	void (*stop)(struct sfp *sfp);
	void (*set_signal_rate)(struct sfp *sfp, unsigned int rate_kbd);
	int (*module_info)(struct sfp *sfp, struct ethtool_modinfo *modinfo);
	int (*module_eeprom)(struct sfp *sfp, struct ethtool_eeprom *ee,
			     u8 *data);
	int (*module_eeprom_by_page)(struct sfp *sfp,
				     const struct ethtool_module_eeprom *page,
				     struct netlink_ext_ack *extack);
};

int sfp_add_phy(struct sfp_bus *bus, struct phy_device *phydev);
void sfp_remove_phy(struct sfp_bus *bus);
void sfp_link_up(struct sfp_bus *bus);
void sfp_link_down(struct sfp_bus *bus);
int sfp_module_insert(struct sfp_bus *bus, const struct sfp_eeprom_id *id,
		      const struct sfp_quirk *quirk);
void sfp_module_remove(struct sfp_bus *bus);
int sfp_module_start(struct sfp_bus *bus);
void sfp_module_stop(struct sfp_bus *bus);
struct sfp_bus *sfp_register_socket(struct device *dev, struct sfp *sfp,
				    const struct sfp_socket_ops *ops);
void sfp_unregister_socket(struct sfp_bus *bus);

#endif
