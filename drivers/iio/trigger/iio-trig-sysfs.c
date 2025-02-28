// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2011 Analog Devices Inc.
 */

#include <freax/kernel.h>
#include <freax/module.h>
#include <freax/platform_device.h>
#include <freax/slab.h>
#include <freax/list.h>
#include <freax/irq_work.h>

#include <freax/iio/iio.h>
#include <freax/iio/trigger.h>

struct iio_sysfs_trig {
	struct iio_trigger *trig;
	struct irq_work work;
	int id;
	struct list_head l;
};

static LIST_HEAD(iio_sysfs_trig_list);
static DEFINE_MUTEX(iio_sysfs_trig_list_mut);

static int iio_sysfs_trigger_probe(int id);
static ssize_t iio_sysfs_trig_add(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf,
				  size_t len)
{
	int ret;
	unsigned long input;

	ret = kstrtoul(buf, 10, &input);
	if (ret)
		return ret;
	ret = iio_sysfs_trigger_probe(input);
	if (ret)
		return ret;
	return len;
}
static DEVICE_ATTR(add_trigger, S_IWUSR, NULL, &iio_sysfs_trig_add);

static int iio_sysfs_trigger_remove(int id);
static ssize_t iio_sysfs_trig_remove(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf,
				     size_t len)
{
	int ret;
	unsigned long input;

	ret = kstrtoul(buf, 10, &input);
	if (ret)
		return ret;
	ret = iio_sysfs_trigger_remove(input);
	if (ret)
		return ret;
	return len;
}

static DEVICE_ATTR(remove_trigger, S_IWUSR, NULL, &iio_sysfs_trig_remove);

static struct attribute *iio_sysfs_trig_attrs[] = {
	&dev_attr_add_trigger.attr,
	&dev_attr_remove_trigger.attr,
	NULL,
};

static const struct attribute_group iio_sysfs_trig_group = {
	.attrs = iio_sysfs_trig_attrs,
};

static const struct attribute_group *iio_sysfs_trig_groups[] = {
	&iio_sysfs_trig_group,
	NULL
};


/* Nothing to actually do upon release */
static void iio_trigger_sysfs_release(struct device *dev)
{
}

static struct device iio_sysfs_trig_dev = {
	.bus = &iio_bus_type,
	.groups = iio_sysfs_trig_groups,
	.release = &iio_trigger_sysfs_release,
};

static void iio_sysfs_trigger_work(struct irq_work *work)
{
	struct iio_sysfs_trig *trig = container_of(work, struct iio_sysfs_trig,
							work);

	iio_trigger_poll(trig->trig);
}

static ssize_t iio_sysfs_trigger_poll(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct iio_trigger *trig = to_iio_trigger(dev);
	struct iio_sysfs_trig *sysfs_trig = iio_trigger_get_drvdata(trig);

	irq_work_queue(&sysfs_trig->work);

	return count;
}

static DEVICE_ATTR(trigger_now, S_IWUSR, NULL, iio_sysfs_trigger_poll);

static struct attribute *iio_sysfs_trigger_attrs[] = {
	&dev_attr_trigger_now.attr,
	NULL,
};

static const struct attribute_group iio_sysfs_trigger_attr_group = {
	.attrs = iio_sysfs_trigger_attrs,
};

static const struct attribute_group *iio_sysfs_trigger_attr_groups[] = {
	&iio_sysfs_trigger_attr_group,
	NULL
};

static int iio_sysfs_trigger_probe(int id)
{
	struct iio_sysfs_trig *t;
	int ret;
	bool foundit = false;

	mutex_lock(&iio_sysfs_trig_list_mut);
	list_for_each_entry(t, &iio_sysfs_trig_list, l)
		if (id == t->id) {
			foundit = true;
			break;
		}
	if (foundit) {
		ret = -EINVAL;
		goto err_unlock;
	}
	t = kmalloc(sizeof(*t), GFP_KERNEL);
	if (t == NULL) {
		ret = -ENOMEM;
		goto err_unlock;
	}
	t->id = id;
	t->trig = iio_trigger_alloc(&iio_sysfs_trig_dev, "sysfstrig%d", id);
	if (!t->trig) {
		ret = -ENOMEM;
		goto err_free_sys_trig;
	}

	t->trig->dev.groups = iio_sysfs_trigger_attr_groups;
	iio_trigger_set_drvdata(t->trig, t);

	t->work = IRQ_WORK_INIT_HARD(iio_sysfs_trigger_work);

	ret = iio_trigger_register(t->trig);
	if (ret)
		goto err_free_trig;
	list_add(&t->l, &iio_sysfs_trig_list);
	__module_get(THIS_MODULE);
	mutex_unlock(&iio_sysfs_trig_list_mut);
	return 0;

err_free_trig:
	iio_trigger_free(t->trig);
err_free_sys_trig:
	kfree(t);
err_unlock:
	mutex_unlock(&iio_sysfs_trig_list_mut);
	return ret;
}

static int iio_sysfs_trigger_remove(int id)
{
	struct iio_sysfs_trig *t = NULL, *iter;

	mutex_lock(&iio_sysfs_trig_list_mut);
	list_for_each_entry(iter, &iio_sysfs_trig_list, l)
		if (id == iter->id) {
			t = iter;
			break;
		}
	if (!t) {
		mutex_unlock(&iio_sysfs_trig_list_mut);
		return -EINVAL;
	}

	iio_trigger_unregister(t->trig);
	irq_work_sync(&t->work);
	iio_trigger_free(t->trig);

	list_del(&t->l);
	kfree(t);
	module_put(THIS_MODULE);
	mutex_unlock(&iio_sysfs_trig_list_mut);
	return 0;
}


static int __init iio_sysfs_trig_init(void)
{
	int ret;
	device_initialize(&iio_sysfs_trig_dev);
	dev_set_name(&iio_sysfs_trig_dev, "iio_sysfs_trigger");
	ret = device_add(&iio_sysfs_trig_dev);
	if (ret)
		put_device(&iio_sysfs_trig_dev);
	return ret;
}
module_init(iio_sysfs_trig_init);

static void __exit iio_sysfs_trig_exit(void)
{
	device_unregister(&iio_sysfs_trig_dev);
}
module_exit(iio_sysfs_trig_exit);

MODULE_AUTHOR("Michael Hennerich <michael.hennerich@analog.com>");
MODULE_DESCRIPTION("Sysfs based trigger for the iio subsystem");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:iio-trig-sysfs");
