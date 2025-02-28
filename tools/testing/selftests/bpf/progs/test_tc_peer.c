// SPDX-License-Identifier: GPL-2.0
#include <stdint.h>
#include <stdbool.h>

#include <freax/bpf.h>
#include <freax/stddef.h>
#include <freax/pkt_cls.h>
#include <freax/if_ether.h>
#include <freax/ip.h>

#include <bpf/bpf_helpers.h>

volatile const __u32 IFINDEX_SRC;
volatile const __u32 IFINDEX_DST;

static const __u8 src_mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const __u8 dst_mac[] = {0x00, 0x22, 0x33, 0x44, 0x55, 0x66};

SEC("tc")
int tc_chk(struct __sk_buff *skb)
{
	return TC_ACT_SHOT;
}

SEC("tc")
int tc_dst(struct __sk_buff *skb)
{
	return bpf_redirect_peer(IFINDEX_SRC, 0);
}

SEC("tc")
int tc_src(struct __sk_buff *skb)
{
	return bpf_redirect_peer(IFINDEX_DST, 0);
}

SEC("tc")
int tc_dst_l3(struct __sk_buff *skb)
{
	return bpf_redirect(IFINDEX_SRC, 0);
}

SEC("tc")
int tc_src_l3(struct __sk_buff *skb)
{
	__u16 proto = skb->protocol;

	if (bpf_skb_change_head(skb, ETH_HLEN, 0) != 0)
		return TC_ACT_SHOT;

	if (bpf_skb_store_bytes(skb, 0, &src_mac, ETH_ALEN, 0) != 0)
		return TC_ACT_SHOT;

	if (bpf_skb_store_bytes(skb, ETH_ALEN, &dst_mac, ETH_ALEN, 0) != 0)
		return TC_ACT_SHOT;

	if (bpf_skb_store_bytes(skb, ETH_ALEN + ETH_ALEN, &proto, sizeof(__u16), 0) != 0)
		return TC_ACT_SHOT;

	return bpf_redirect_peer(IFINDEX_DST, 0);
}

char __license[] SEC("license") = "GPL";
