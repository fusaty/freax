// SPDX-License-Identifier: GPL-2.0

#include <freax/interconnect-provider.h>
#include <freax/device.h>
#include <freax/export.h>

/**
 * of_icc_bulk_get() - get interconnect paths
 * @dev: the device requesting the path
 * @num_paths: the number of icc_bulk_data
 * @paths: the table with the paths we want to get
 *
 * Returns 0 on success or negative errno otherwise.
 */
int __must_check of_icc_bulk_get(struct device *dev, int num_paths,
				 struct icc_bulk_data *paths)
{
	int ret, i;

	for (i = 0; i < num_paths; i++) {
		paths[i].path = of_icc_get(dev, paths[i].name);
		if (IS_ERR(paths[i].path)) {
			ret = PTR_ERR(paths[i].path);
			if (ret != -EPROBE_DEFER)
				dev_err(dev, "of_icc_get() failed on path %s (%d)\n",
					paths[i].name, ret);
			paths[i].path = NULL;
			goto err;
		}
	}

	return 0;

err:
	icc_bulk_put(i, paths);

	return ret;
}
EXPORT_SYMBOL_GPL(of_icc_bulk_get);

/**
 * icc_bulk_put() - put a list of interconnect paths
 * @num_paths: the number of icc_bulk_data
 * @paths: the icc_bulk_data table with the paths being put
 */
void icc_bulk_put(int num_paths, struct icc_bulk_data *paths)
{
	while (--num_paths >= 0) {
		icc_put(paths[num_paths].path);
		paths[num_paths].path = NULL;
	}
}
EXPORT_SYMBOL_GPL(icc_bulk_put);

/**
 * icc_bulk_set_bw() - set bandwidth to a set of paths
 * @num_paths: the number of icc_bulk_data
 * @paths: the icc_bulk_data table containing the paths and bandwidth
 *
 * Returns 0 on success or negative errno otherwise.
 */
int icc_bulk_set_bw(int num_paths, const struct icc_bulk_data *paths)
{
	int ret = 0;
	int i;

	for (i = 0; i < num_paths; i++) {
		ret = icc_set_bw(paths[i].path, paths[i].avg_bw, paths[i].peak_bw);
		if (ret) {
			pr_err("icc_set_bw() failed on path %s (%d)\n", paths[i].name, ret);
			return ret;
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(icc_bulk_set_bw);

/**
 * icc_bulk_enable() - enable a previously disabled set of paths
 * @num_paths: the number of icc_bulk_data
 * @paths: the icc_bulk_data table containing the paths and bandwidth
 *
 * Returns 0 on success or negative errno otherwise.
 */
int icc_bulk_enable(int num_paths, const struct icc_bulk_data *paths)
{
	int ret, i;

	for (i = 0; i < num_paths; i++) {
		ret = icc_enable(paths[i].path);
		if (ret) {
			pr_err("icc_enable() failed on path %s (%d)\n", paths[i].name, ret);
			goto err;
		}
	}

	return 0;

err:
	icc_bulk_disable(i, paths);

	return ret;
}
EXPORT_SYMBOL_GPL(icc_bulk_enable);

/**
 * icc_bulk_disable() - disable a set of interconnect paths
 * @num_paths: the number of icc_bulk_data
 * @paths: the icc_bulk_data table containing the paths and bandwidth
 */
void icc_bulk_disable(int num_paths, const struct icc_bulk_data *paths)
{
	while (--num_paths >= 0)
		icc_disable(paths[num_paths].path);
}
EXPORT_SYMBOL_GPL(icc_bulk_disable);

struct icc_bulk_devres {
	struct icc_bulk_data *paths;
	int num_paths;
};

static void devm_icc_bulk_release(struct device *dev, void *res)
{
	struct icc_bulk_devres *devres = res;

	icc_bulk_put(devres->num_paths, devres->paths);
}

/**
 * devm_of_icc_bulk_get() - resource managed of_icc_bulk_get
 * @dev: the device requesting the path
 * @num_paths: the number of icc_bulk_data
 * @paths: the table with the paths we want to get
 *
 * Returns 0 on success or negative errno otherwise.
 */
int devm_of_icc_bulk_get(struct device *dev, int num_paths, struct icc_bulk_data *paths)
{
	struct icc_bulk_devres *devres;
	int ret;

	devres = devres_alloc(devm_icc_bulk_release, sizeof(*devres), GFP_KERNEL);
	if (!devres)
		return -ENOMEM;

	ret = of_icc_bulk_get(dev, num_paths, paths);
	if (!ret) {
		devres->paths = paths;
		devres->num_paths = num_paths;
		devres_add(dev, devres);
	} else {
		devres_free(devres);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(devm_of_icc_bulk_get);
