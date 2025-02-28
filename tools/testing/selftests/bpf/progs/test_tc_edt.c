// SPDX-License-Identifier: GPL-2.0
#include <stdint.h>
#include <freax/bpf.h>
#include <freax/if_ether.h>
#include <freax/stddef.h>
#include <freax/in.h>
#include <freax/ip.h>
#include <freax/pkt_cls.h>
#include <freax/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

/* the maximum delay we are willing to add (drop packets beyond that) */
#define TIME_HORIZON_NS (2000 * 1000 * 1000)
#define NS_PER_SEC 1000000000
#define ECN_HORIZON_NS 5000000
#define THROTTLE_RATE_BPS (5 * 1000 * 1000)

/* flow_key => last_tstamp timestamp used */
struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__type(key, uint32_t);
	__type(value, uint64_t);
	__uint(max_entries, 1);
} flow_map SEC(".maps");

static inline int throttle_flow(struct __sk_buff *skb)
{
	int key = 0;
	uint64_t *last_tstamp = bpf_map_lookup_elem(&flow_map, &key);
	uint64_t delay_ns = ((uint64_t)skb->len) * NS_PER_SEC /
			THROTTLE_RATE_BPS;
	uint64_t now = bpf_ktime_get_ns();
	uint64_t tstamp, next_tstamp = 0;

	if (last_tstamp)
		next_tstamp = *last_tstamp + delay_ns;

	tstamp = skb->tstamp;
	if (tstamp < now)
		tstamp = now;

	/* should we throttle? */
	if (next_tstamp <= tstamp) {
		if (bpf_map_update_elem(&flow_map, &key, &tstamp, BPF_ANY))
			return TC_ACT_SHOT;
		return TC_ACT_OK;
	}

	/* do not queue past the time horizon */
	if (next_tstamp - now >= TIME_HORIZON_NS)
		return TC_ACT_SHOT;

	/* set ecn bit, if needed */
	if (next_tstamp - now >= ECN_HORIZON_NS)
		bpf_skb_ecn_set_ce(skb);

	if (bpf_map_update_elem(&flow_map, &key, &next_tstamp, BPF_EXIST))
		return TC_ACT_SHOT;
	skb->tstamp = next_tstamp;

	return TC_ACT_OK;
}

static inline int handle_tcp(struct __sk_buff *skb, struct tcphdr *tcp)
{
	void *data_end = (void *)(long)skb->data_end;

	/* drop malformed packets */
	if ((void *)(tcp + 1) > data_end)
		return TC_ACT_SHOT;

	if (tcp->dest == bpf_htons(9000))
		return throttle_flow(skb);

	return TC_ACT_OK;
}

static inline int handle_ipv4(struct __sk_buff *skb)
{
	void *data_end = (void *)(long)skb->data_end;
	void *data = (void *)(long)skb->data;
	struct iphdr *iph;
	uint32_t ihl;

	/* drop malformed packets */
	if (data + sizeof(struct ethhdr) > data_end)
		return TC_ACT_SHOT;
	iph = (struct iphdr *)(data + sizeof(struct ethhdr));
	if ((void *)(iph + 1) > data_end)
		return TC_ACT_SHOT;
	ihl = iph->ihl * 4;
	if (((void *)iph) + ihl > data_end)
		return TC_ACT_SHOT;

	if (iph->protocol == IPPROTO_TCP)
		return handle_tcp(skb, (struct tcphdr *)(((void *)iph) + ihl));

	return TC_ACT_OK;
}

SEC("cls_test") int tc_prog(struct __sk_buff *skb)
{
	if (skb->protocol == bpf_htons(ETH_P_IP))
		return handle_ipv4(skb);

	return TC_ACT_OK;
}

char __license[] SEC("license") = "GPL";
