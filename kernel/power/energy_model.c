// SPDX-License-Identifier: GPL-2.0
/*
 * Energy Model of devices
 *
 * Copyright (c) 2018-2021, Arm ltd.
 * Written by: Quentin Perret, Arm ltd.
 * Improvements provided by: Lukasz Luba, Arm ltd.
 */

#define pr_fmt(fmt) "energy_model: " fmt

#include <freax/cpu.h>
#include <freax/cpufreq.h>
#include <freax/cpumask.h>
#include <freax/debugfs.h>
#include <freax/energy_model.h>
#include <freax/sched/topology.h>
#include <freax/slab.h>

/*
 * Mutex serializing the registrations of performance domains and letting
 * callbacks defined by drivers sleep.
 */
static DEFINE_MUTEX(em_pd_mutex);

static bool _is_cpu_device(struct device *dev)
{
	return (dev->bus == &cpu_subsys);
}

#ifdef CONFIG_DEBUG_FS
static struct dentry *rootdir;

static void em_debug_create_ps(struct em_perf_state *ps, struct dentry *pd)
{
	struct dentry *d;
	char name[24];

	snprintf(name, sizeof(name), "ps:%lu", ps->frequency);

	/* Create per-ps directory */
	d = debugfs_create_dir(name, pd);
	debugfs_create_ulong("frequency", 0444, d, &ps->frequency);
	debugfs_create_ulong("power", 0444, d, &ps->power);
	debugfs_create_ulong("cost", 0444, d, &ps->cost);
	debugfs_create_ulong("inefficient", 0444, d, &ps->flags);
}

static int em_debug_cpus_show(struct seq_file *s, void *unused)
{
	seq_printf(s, "%*pbl\n", cpumask_pr_args(to_cpumask(s->private)));

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(em_debug_cpus);

static int em_debug_flags_show(struct seq_file *s, void *unused)
{
	struct em_perf_domain *pd = s->private;

	seq_printf(s, "%#lx\n", pd->flags);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(em_debug_flags);

static void em_debug_create_pd(struct device *dev)
{
	struct dentry *d;
	int i;

	/* Create the directory of the performance domain */
	d = debugfs_create_dir(dev_name(dev), rootdir);

	if (_is_cpu_device(dev))
		debugfs_create_file("cpus", 0444, d, dev->em_pd->cpus,
				    &em_debug_cpus_fops);

	debugfs_create_file("flags", 0444, d, dev->em_pd,
			    &em_debug_flags_fops);

	/* Create a sub-directory for each performance state */
	for (i = 0; i < dev->em_pd->nr_perf_states; i++)
		em_debug_create_ps(&dev->em_pd->table[i], d);

}

static void em_debug_remove_pd(struct device *dev)
{
	debugfs_lookup_and_remove(dev_name(dev), rootdir);
}

static int __init em_debug_init(void)
{
	/* Create /sys/kernel/debug/energy_model directory */
	rootdir = debugfs_create_dir("energy_model", NULL);

	return 0;
}
fs_initcall(em_debug_init);
#else /* CONFIG_DEBUG_FS */
static void em_debug_create_pd(struct device *dev) {}
static void em_debug_remove_pd(struct device *dev) {}
#endif

static int em_create_perf_table(struct device *dev, struct em_perf_domain *pd,
				int nr_states, struct em_data_callback *cb,
				unsigned long flags)
{
	unsigned long power, freq, prev_freq = 0, prev_cost = ULONG_MAX;
	struct em_perf_state *table;
	int i, ret;
	u64 fmax;

	table = kcalloc(nr_states, sizeof(*table), GFP_KERNEL);
	if (!table)
		return -ENOMEM;

	/* Build the list of performance states for this performance domain */
	for (i = 0, freq = 0; i < nr_states; i++, freq++) {
		/*
		 * active_power() is a driver callback which ceils 'freq' to
		 * lowest performance state of 'dev' above 'freq' and updates
		 * 'power' and 'freq' accordingly.
		 */
		ret = cb->active_power(dev, &power, &freq);
		if (ret) {
			dev_err(dev, "EM: invalid perf. state: %d\n",
				ret);
			goto free_ps_table;
		}

		/*
		 * We expect the driver callback to increase the frequency for
		 * higher performance states.
		 */
		if (freq <= prev_freq) {
			dev_err(dev, "EM: non-increasing freq: %lu\n",
				freq);
			goto free_ps_table;
		}

		/*
		 * The power returned by active_state() is expected to be
		 * positive and be in range.
		 */
		if (!power || power > EM_MAX_POWER) {
			dev_err(dev, "EM: invalid power: %lu\n",
				power);
			goto free_ps_table;
		}

		table[i].power = power;
		table[i].frequency = prev_freq = freq;
	}

	/* Compute the cost of each performance state. */
	fmax = (u64) table[nr_states - 1].frequency;
	for (i = nr_states - 1; i >= 0; i--) {
		unsigned long power_res, cost;

		if (flags & EM_PERF_DOMAIN_ARTIFICIAL) {
			ret = cb->get_cost(dev, table[i].frequency, &cost);
			if (ret || !cost || cost > EM_MAX_POWER) {
				dev_err(dev, "EM: invalid cost %lu %d\n",
					cost, ret);
				goto free_ps_table;
			}
		} else {
			power_res = table[i].power;
			cost = div64_u64(fmax * power_res, table[i].frequency);
		}

		table[i].cost = cost;

		if (table[i].cost >= prev_cost) {
			table[i].flags = EM_PERF_STATE_INEFFICIENT;
			dev_dbg(dev, "EM: OPP:%lu is inefficient\n",
				table[i].frequency);
		} else {
			prev_cost = table[i].cost;
		}
	}

	pd->table = table;
	pd->nr_perf_states = nr_states;

	return 0;

free_ps_table:
	kfree(table);
	return -EINVAL;
}

static int em_create_pd(struct device *dev, int nr_states,
			struct em_data_callback *cb, cpumask_t *cpus,
			unsigned long flags)
{
	struct em_perf_domain *pd;
	struct device *cpu_dev;
	int cpu, ret, num_cpus;

	if (_is_cpu_device(dev)) {
		num_cpus = cpumask_weight(cpus);

		/* Prevent max possible energy calculation to not overflow */
		if (num_cpus > EM_MAX_NUM_CPUS) {
			dev_err(dev, "EM: too many CPUs, overflow possible\n");
			return -EINVAL;
		}

		pd = kzalloc(sizeof(*pd) + cpumask_size(), GFP_KERNEL);
		if (!pd)
			return -ENOMEM;

		cpumask_copy(em_span_cpus(pd), cpus);
	} else {
		pd = kzalloc(sizeof(*pd), GFP_KERNEL);
		if (!pd)
			return -ENOMEM;
	}

	ret = em_create_perf_table(dev, pd, nr_states, cb, flags);
	if (ret) {
		kfree(pd);
		return ret;
	}

	if (_is_cpu_device(dev))
		for_each_cpu(cpu, cpus) {
			cpu_dev = get_cpu_device(cpu);
			cpu_dev->em_pd = pd;
		}

	dev->em_pd = pd;

	return 0;
}

static void em_cpufreq_update_efficiencies(struct device *dev)
{
	struct em_perf_domain *pd = dev->em_pd;
	struct em_perf_state *table;
	struct cpufreq_policy *policy;
	int found = 0;
	int i;

	if (!_is_cpu_device(dev) || !pd)
		return;

	policy = cpufreq_cpu_get(cpumask_first(em_span_cpus(pd)));
	if (!policy) {
		dev_warn(dev, "EM: Access to CPUFreq policy failed");
		return;
	}

	table = pd->table;

	for (i = 0; i < pd->nr_perf_states; i++) {
		if (!(table[i].flags & EM_PERF_STATE_INEFFICIENT))
			continue;

		if (!cpufreq_table_set_inefficient(policy, table[i].frequency))
			found++;
	}

	cpufreq_cpu_put(policy);

	if (!found)
		return;

	/*
	 * Efficiencies have been installed in CPUFreq, inefficient frequencies
	 * will be skipped. The EM can do the same.
	 */
	pd->flags |= EM_PERF_DOMAIN_SKIP_INEFFICIENCIES;
}

/**
 * em_pd_get() - Return the performance domain for a device
 * @dev : Device to find the performance domain for
 *
 * Returns the performance domain to which @dev belongs, or NULL if it doesn't
 * exist.
 */
struct em_perf_domain *em_pd_get(struct device *dev)
{
	if (IS_ERR_OR_NULL(dev))
		return NULL;

	return dev->em_pd;
}
EXPORT_SYMBOL_GPL(em_pd_get);

/**
 * em_cpu_get() - Return the performance domain for a CPU
 * @cpu : CPU to find the performance domain for
 *
 * Returns the performance domain to which @cpu belongs, or NULL if it doesn't
 * exist.
 */
struct em_perf_domain *em_cpu_get(int cpu)
{
	struct device *cpu_dev;

	cpu_dev = get_cpu_device(cpu);
	if (!cpu_dev)
		return NULL;

	return em_pd_get(cpu_dev);
}
EXPORT_SYMBOL_GPL(em_cpu_get);

/**
 * em_dev_register_perf_domain() - Register the Energy Model (EM) for a device
 * @dev		: Device for which the EM is to register
 * @nr_states	: Number of performance states to register
 * @cb		: Callback functions providing the data of the Energy Model
 * @cpus	: Pointer to cpumask_t, which in case of a CPU device is
 *		obligatory. It can be taken from i.e. 'policy->cpus'. For other
 *		type of devices this should be set to NULL.
 * @microwatts	: Flag indicating that the power values are in micro-Watts or
 *		in some other scale. It must be set properly.
 *
 * Create Energy Model tables for a performance domain using the callbacks
 * defined in cb.
 *
 * The @microwatts is important to set with correct value. Some kernel
 * sub-systems might rely on this flag and check if all devices in the EM are
 * using the same scale.
 *
 * If multiple clients register the same performance domain, all but the first
 * registration will be ignored.
 *
 * Return 0 on success
 */
int em_dev_register_perf_domain(struct device *dev, unsigned int nr_states,
				struct em_data_callback *cb, cpumask_t *cpus,
				bool microwatts)
{
	unsigned long cap, prev_cap = 0;
	unsigned long flags = 0;
	int cpu, ret;

	if (!dev || !nr_states || !cb)
		return -EINVAL;

	/*
	 * Use a mutex to serialize the registration of performance domains and
	 * let the driver-defined callback functions sleep.
	 */
	mutex_lock(&em_pd_mutex);

	if (dev->em_pd) {
		ret = -EEXIST;
		goto unlock;
	}

	if (_is_cpu_device(dev)) {
		if (!cpus) {
			dev_err(dev, "EM: invalid CPU mask\n");
			ret = -EINVAL;
			goto unlock;
		}

		for_each_cpu(cpu, cpus) {
			if (em_cpu_get(cpu)) {
				dev_err(dev, "EM: exists for CPU%d\n", cpu);
				ret = -EEXIST;
				goto unlock;
			}
			/*
			 * All CPUs of a domain must have the same
			 * micro-architecture since they all share the same
			 * table.
			 */
			cap = arch_scale_cpu_capacity(cpu);
			if (prev_cap && prev_cap != cap) {
				dev_err(dev, "EM: CPUs of %*pbl must have the same capacity\n",
					cpumask_pr_args(cpus));

				ret = -EINVAL;
				goto unlock;
			}
			prev_cap = cap;
		}
	}

	if (microwatts)
		flags |= EM_PERF_DOMAIN_MICROWATTS;
	else if (cb->get_cost)
		flags |= EM_PERF_DOMAIN_ARTIFICIAL;

	ret = em_create_pd(dev, nr_states, cb, cpus, flags);
	if (ret)
		goto unlock;

	dev->em_pd->flags |= flags;

	em_cpufreq_update_efficiencies(dev);

	em_debug_create_pd(dev);
	dev_info(dev, "EM: created perf domain\n");

unlock:
	mutex_unlock(&em_pd_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(em_dev_register_perf_domain);

/**
 * em_dev_unregister_perf_domain() - Unregister Energy Model (EM) for a device
 * @dev		: Device for which the EM is registered
 *
 * Unregister the EM for the specified @dev (but not a CPU device).
 */
void em_dev_unregister_perf_domain(struct device *dev)
{
	if (IS_ERR_OR_NULL(dev) || !dev->em_pd)
		return;

	if (_is_cpu_device(dev))
		return;

	/*
	 * The mutex separates all register/unregister requests and protects
	 * from potential clean-up/setup issues in the debugfs directories.
	 * The debugfs directory name is the same as device's name.
	 */
	mutex_lock(&em_pd_mutex);
	em_debug_remove_pd(dev);

	kfree(dev->em_pd->table);
	kfree(dev->em_pd);
	dev->em_pd = NULL;
	mutex_unlock(&em_pd_mutex);
}
EXPORT_SYMBOL_GPL(em_dev_unregister_perf_domain);
