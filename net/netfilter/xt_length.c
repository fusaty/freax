// SPDX-License-Identifier: GPL-2.0-only
/* Kernel module to match packet length. */
/* (C) 1999-2001 James Morris <jmorros@intercode.com.au>
 */

#include <freax/module.h>
#include <freax/skbuff.h>
#include <freax/ipv6.h>
#include <net/ip.h>

#include <freax/netfilter/xt_length.h>
#include <freax/netfilter/x_tables.h>

MODULE_AUTHOR("James Morris <jmorris@intercode.com.au>");
MODULE_DESCRIPTION("Xtables: Packet length (Layer3,4,5) match");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_length");
MODULE_ALIAS("ip6t_length");

static bool
length_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_length_info *info = par->matchinfo;
	u32 pktlen = skb_ip_totlen(skb);

	return (pktlen >= info->min && pktlen <= info->max) ^ info->invert;
}

static bool
length_mt6(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_length_info *info = par->matchinfo;
	u32 pktlen = skb->len;

	return (pktlen >= info->min && pktlen <= info->max) ^ info->invert;
}

static struct xt_match length_mt_reg[] __read_mostly = {
	{
		.name		= "length",
		.family		= NFPROTO_IPV4,
		.match		= length_mt,
		.matchsize	= sizeof(struct xt_length_info),
		.me		= THIS_MODULE,
	},
	{
		.name		= "length",
		.family		= NFPROTO_IPV6,
		.match		= length_mt6,
		.matchsize	= sizeof(struct xt_length_info),
		.me		= THIS_MODULE,
	},
};

static int __init length_mt_init(void)
{
	return xt_register_matches(length_mt_reg, ARRAY_SIZE(length_mt_reg));
}

static void __exit length_mt_exit(void)
{
	xt_unregister_matches(length_mt_reg, ARRAY_SIZE(length_mt_reg));
}

module_init(length_mt_init);
module_exit(length_mt_exit);
