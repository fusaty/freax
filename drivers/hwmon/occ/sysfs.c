// SPDX-License-Identifier: GPL-2.0+
// Copyright IBM Corp 2019

#include <freax/bitops.h>
#include <freax/device.h>
#include <freax/export.h>
#include <freax/hwmon-sysfs.h>
#include <freax/kernel.h>
#include <freax/kstrtox.h>
#include <freax/sysfs.h>

#include "common.h"

/* OCC status register */
#define OCC_STAT_MASTER			BIT(7)

/* OCC extended status register */
#define OCC_EXT_STAT_DVFS_OT		BIT(7)
#define OCC_EXT_STAT_DVFS_POWER		BIT(6)
#define OCC_EXT_STAT_MEM_THROTTLE	BIT(5)
#define OCC_EXT_STAT_QUICK_DROP		BIT(4)
#define OCC_EXT_STAT_DVFS_VDD		BIT(3)
#define OCC_EXT_STAT_GPU_THROTTLE	GENMASK(2, 0)

static ssize_t occ_active_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	int rc;
	bool active;
	struct occ *occ = dev_get_drvdata(dev);

	rc = kstrtobool(buf, &active);
	if (rc)
		return rc;

	rc = occ_active(occ, active);
	if (rc)
		return rc;

	return count;
}

static ssize_t occ_sysfs_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	int rc;
	int val = 0;
	struct occ *occ = dev_get_drvdata(dev);
	struct occ_poll_response_header *header;
	struct sensor_device_attribute *sattr = to_sensor_dev_attr(attr);

	if (occ->active) {
		rc = occ_update_response(occ);
		if (rc)
			return rc;

		header = (struct occ_poll_response_header *)occ->resp.data;

		switch (sattr->index) {
		case 0:
			val = !!(header->status & OCC_STAT_MASTER);
			break;
		case 1:
			val = 1;
			break;
		case 2:
			val = !!(header->ext_status & OCC_EXT_STAT_DVFS_OT);
			break;
		case 3:
			val = !!(header->ext_status & OCC_EXT_STAT_DVFS_POWER);
			break;
		case 4:
			val = !!(header->ext_status &
				 OCC_EXT_STAT_MEM_THROTTLE);
			break;
		case 5:
			val = !!(header->ext_status & OCC_EXT_STAT_QUICK_DROP);
			break;
		case 6:
			val = header->occ_state;
			break;
		case 7:
			if (header->status & OCC_STAT_MASTER)
				val = hweight8(header->occs_present);
			else
				val = 1;
			break;
		case 8:
			val = header->ips_status;
			break;
		case 9:
			val = header->mode;
			break;
		case 10:
			val = !!(header->ext_status & OCC_EXT_STAT_DVFS_VDD);
			break;
		case 11:
			val = header->ext_status & OCC_EXT_STAT_GPU_THROTTLE;
			break;
		default:
			return -EINVAL;
		}
	} else {
		if (sattr->index == 1)
			val = 0;
		else if (sattr->index <= 11)
			val = -ENODATA;
		else
			return -EINVAL;
	}

	return sysfs_emit(buf, "%d\n", val);
}

static ssize_t occ_error_show(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct occ *occ = dev_get_drvdata(dev);

	occ_update_response(occ);

	return sysfs_emit(buf, "%d\n", occ->error);
}

static SENSOR_DEVICE_ATTR(occ_master, 0444, occ_sysfs_show, NULL, 0);
static SENSOR_DEVICE_ATTR(occ_active, 0644, occ_sysfs_show, occ_active_store,
			  1);
static SENSOR_DEVICE_ATTR(occ_dvfs_overtemp, 0444, occ_sysfs_show, NULL, 2);
static SENSOR_DEVICE_ATTR(occ_dvfs_power, 0444, occ_sysfs_show, NULL, 3);
static SENSOR_DEVICE_ATTR(occ_mem_throttle, 0444, occ_sysfs_show, NULL, 4);
static SENSOR_DEVICE_ATTR(occ_quick_pwr_drop, 0444, occ_sysfs_show, NULL, 5);
static SENSOR_DEVICE_ATTR(occ_state, 0444, occ_sysfs_show, NULL, 6);
static SENSOR_DEVICE_ATTR(occs_present, 0444, occ_sysfs_show, NULL, 7);
static SENSOR_DEVICE_ATTR(occ_ips_status, 0444, occ_sysfs_show, NULL, 8);
static SENSOR_DEVICE_ATTR(occ_mode, 0444, occ_sysfs_show, NULL, 9);
static SENSOR_DEVICE_ATTR(occ_dvfs_vdd, 0444, occ_sysfs_show, NULL, 10);
static SENSOR_DEVICE_ATTR(occ_gpu_throttle, 0444, occ_sysfs_show, NULL, 11);
static DEVICE_ATTR_RO(occ_error);

static struct attribute *occ_attributes[] = {
	&sensor_dev_attr_occ_master.dev_attr.attr,
	&sensor_dev_attr_occ_active.dev_attr.attr,
	&sensor_dev_attr_occ_dvfs_overtemp.dev_attr.attr,
	&sensor_dev_attr_occ_dvfs_power.dev_attr.attr,
	&sensor_dev_attr_occ_mem_throttle.dev_attr.attr,
	&sensor_dev_attr_occ_quick_pwr_drop.dev_attr.attr,
	&sensor_dev_attr_occ_state.dev_attr.attr,
	&sensor_dev_attr_occs_present.dev_attr.attr,
	&sensor_dev_attr_occ_ips_status.dev_attr.attr,
	&sensor_dev_attr_occ_mode.dev_attr.attr,
	&sensor_dev_attr_occ_dvfs_vdd.dev_attr.attr,
	&sensor_dev_attr_occ_gpu_throttle.dev_attr.attr,
	&dev_attr_occ_error.attr,
	NULL
};

static const struct attribute_group occ_sysfs = {
	.attrs = occ_attributes,
};

void occ_sysfs_poll_done(struct occ *occ)
{
	const char *name;
	struct occ_poll_response_header *header =
		(struct occ_poll_response_header *)occ->resp.data;

	/*
	 * On the first poll response, we haven't yet created the sysfs
	 * attributes, so don't make any notify calls.
	 */
	if (!occ->active)
		goto done;

	if ((header->status & OCC_STAT_MASTER) !=
	    (occ->prev_stat & OCC_STAT_MASTER)) {
		name = sensor_dev_attr_occ_master.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_DVFS_OT) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_DVFS_OT)) {
		name = sensor_dev_attr_occ_dvfs_overtemp.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_DVFS_POWER) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_DVFS_POWER)) {
		name = sensor_dev_attr_occ_dvfs_power.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_MEM_THROTTLE) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_MEM_THROTTLE)) {
		name = sensor_dev_attr_occ_mem_throttle.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_QUICK_DROP) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_QUICK_DROP)) {
		name = sensor_dev_attr_occ_quick_pwr_drop.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_DVFS_VDD) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_DVFS_VDD)) {
		name = sensor_dev_attr_occ_dvfs_vdd.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->ext_status & OCC_EXT_STAT_GPU_THROTTLE) !=
	    (occ->prev_ext_stat & OCC_EXT_STAT_GPU_THROTTLE)) {
		name = sensor_dev_attr_occ_gpu_throttle.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if ((header->status & OCC_STAT_MASTER) &&
	    header->occs_present != occ->prev_occs_present) {
		name = sensor_dev_attr_occs_present.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if (header->ips_status != occ->prev_ips_status) {
		name = sensor_dev_attr_occ_ips_status.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if (header->mode != occ->prev_mode) {
		name = sensor_dev_attr_occ_mode.dev_attr.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	if (occ->error && occ->error != occ->prev_error) {
		name = dev_attr_occ_error.attr.name;
		sysfs_notify(&occ->bus_dev->kobj, NULL, name);
	}

	/* no notifications for OCC state; doesn't indicate error condition */

done:
	occ->prev_error = occ->error;
	occ->prev_stat = header->status;
	occ->prev_ext_stat = header->ext_status;
	occ->prev_occs_present = header->occs_present;
	occ->prev_ips_status = header->ips_status;
	occ->prev_mode = header->mode;
}

int occ_setup_sysfs(struct occ *occ)
{
	return sysfs_create_group(&occ->bus_dev->kobj, &occ_sysfs);
}

void occ_shutdown_sysfs(struct occ *occ)
{
	sysfs_remove_group(&occ->bus_dev->kobj, &occ_sysfs);
}
