// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright 2020 Google LLC.
 */

#include <errno.h>
#include <freax/bpf.h>
#include <freax/ip.h>
#include <freax/udp.h>
#include <bpf/bpf_helpers.h>

#include "progs/cg_storage_multi.h"

struct {
	__uint(type, BPF_MAP_TYPE_CGROUP_STORAGE);
	__type(key, __u64);
	__type(value, struct cgroup_value);
} cgroup_storage SEC(".maps");

__u32 invocations = 0;

SEC("cgroup_skb/egress")
int egress1(struct __sk_buff *skb)
{
	struct cgroup_value *ptr_cg_storage =
		bpf_get_local_storage(&cgroup_storage, 0);

	__sync_fetch_and_add(&ptr_cg_storage->egress_pkts, 1);
	__sync_fetch_and_add(&invocations, 1);

	return 1;
}

SEC("cgroup_skb/egress")
int egress2(struct __sk_buff *skb)
{
	struct cgroup_value *ptr_cg_storage =
		bpf_get_local_storage(&cgroup_storage, 0);

	__sync_fetch_and_add(&ptr_cg_storage->egress_pkts, 1);
	__sync_fetch_and_add(&invocations, 1);

	return 1;
}

SEC("cgroup_skb/ingress")
int ingress(struct __sk_buff *skb)
{
	struct cgroup_value *ptr_cg_storage =
		bpf_get_local_storage(&cgroup_storage, 0);

	__sync_fetch_and_add(&ptr_cg_storage->ingress_pkts, 1);
	__sync_fetch_and_add(&invocations, 1);

	return 1;
}
