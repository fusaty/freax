// SPDX-License-Identifier: GPL-2.0
#include <freax/types.h>

#include <xen/xen.h>
#include <xen/hvm.h>
#include <xen/features.h>
#include <xen/interface/features.h>
#include <xen/events.h>

#include "xen-ops.h"

void xen_hvm_post_suspend(int suspend_cancelled)
{
	if (!suspend_cancelled) {
		xen_hvm_init_shared_info();
		xen_vcpu_restore();
	}
	if (xen_percpu_upcall) {
		unsigned int cpu;

		for_each_online_cpu(cpu)
			BUG_ON(xen_set_upcall_vector(cpu));
	} else {
		xen_setup_callback_vector();
	}
	xen_unplug_emulated_devices();
}
