// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2013 Eric Leblond <eric@regit.org>
 *
 * Development of this code partly funded by OISF
 * (http://www.openinfosecfoundation.org/)
 */

#include <freax/kernel.h>
#include <freax/init.h>
#include <freax/module.h>
#include <freax/netlink.h>
#include <freax/jhash.h>
#include <freax/netfilter.h>
#include <freax/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/nf_queue.h>

static u32 jhash_initval __read_mostly;

struct nft_queue {
	u8	sreg_qnum;
	u16	queuenum;
	u16	queues_total;
	u16	flags;
};

static void nft_queue_eval(const struct nft_expr *expr,
			   struct nft_regs *regs,
			   const struct nft_pktinfo *pkt)
{
	struct nft_queue *priv = nft_expr_priv(expr);
	u32 queue = priv->queuenum;
	u32 ret;

	if (priv->queues_total > 1) {
		if (priv->flags & NFT_QUEUE_FLAG_CPU_FANOUT) {
			int cpu = raw_smp_processor_id();

			queue = priv->queuenum + cpu % priv->queues_total;
		} else {
			queue = nfqueue_hash(pkt->skb, queue,
					     priv->queues_total, nft_pf(pkt),
					     jhash_initval);
		}
	}

	ret = NF_QUEUE_NR(queue);
	if (priv->flags & NFT_QUEUE_FLAG_BYPASS)
		ret |= NF_VERDICT_FLAG_QUEUE_BYPASS;

	regs->verdict.code = ret;
}

static void nft_queue_sreg_eval(const struct nft_expr *expr,
				struct nft_regs *regs,
				const struct nft_pktinfo *pkt)
{
	struct nft_queue *priv = nft_expr_priv(expr);
	u32 queue, ret;

	queue = regs->data[priv->sreg_qnum];

	ret = NF_QUEUE_NR(queue);
	if (priv->flags & NFT_QUEUE_FLAG_BYPASS)
		ret |= NF_VERDICT_FLAG_QUEUE_BYPASS;

	regs->verdict.code = ret;
}

static int nft_queue_validate(const struct nft_ctx *ctx,
			      const struct nft_expr *expr,
			      const struct nft_data **data)
{
	static const unsigned int supported_hooks = ((1 << NF_INET_PRE_ROUTING) |
						     (1 << NF_INET_LOCAL_IN) |
						     (1 << NF_INET_FORWARD) |
						     (1 << NF_INET_LOCAL_OUT) |
						     (1 << NF_INET_POST_ROUTING));

	switch (ctx->family) {
	case NFPROTO_IPV4:
	case NFPROTO_IPV6:
	case NFPROTO_INET:
	case NFPROTO_BRIDGE:
		break;
	case NFPROTO_NETDEV: /* lacks okfn */
		fallthrough;
	default:
		return -EOPNOTSUPP;
	}

	return nft_chain_validate_hooks(ctx->chain, supported_hooks);
}

static const struct nla_policy nft_queue_policy[NFTA_QUEUE_MAX + 1] = {
	[NFTA_QUEUE_NUM]	= { .type = NLA_U16 },
	[NFTA_QUEUE_TOTAL]	= { .type = NLA_U16 },
	[NFTA_QUEUE_FLAGS]	= { .type = NLA_U16 },
	[NFTA_QUEUE_SREG_QNUM]	= { .type = NLA_U32 },
};

static int nft_queue_init(const struct nft_ctx *ctx,
			  const struct nft_expr *expr,
			  const struct nlattr * const tb[])
{
	struct nft_queue *priv = nft_expr_priv(expr);
	u32 maxid;

	priv->queuenum = ntohs(nla_get_be16(tb[NFTA_QUEUE_NUM]));

	if (tb[NFTA_QUEUE_TOTAL])
		priv->queues_total = ntohs(nla_get_be16(tb[NFTA_QUEUE_TOTAL]));
	else
		priv->queues_total = 1;

	if (priv->queues_total == 0)
		return -EINVAL;

	maxid = priv->queues_total - 1 + priv->queuenum;
	if (maxid > U16_MAX)
		return -ERANGE;

	if (tb[NFTA_QUEUE_FLAGS]) {
		priv->flags = ntohs(nla_get_be16(tb[NFTA_QUEUE_FLAGS]));
		if (priv->flags & ~NFT_QUEUE_FLAG_MASK)
			return -EINVAL;
	}
	return 0;
}

static int nft_queue_sreg_init(const struct nft_ctx *ctx,
			       const struct nft_expr *expr,
			       const struct nlattr * const tb[])
{
	struct nft_queue *priv = nft_expr_priv(expr);
	int err;

	err = nft_parse_register_load(tb[NFTA_QUEUE_SREG_QNUM],
				      &priv->sreg_qnum, sizeof(u32));
	if (err < 0)
		return err;

	if (tb[NFTA_QUEUE_FLAGS]) {
		priv->flags = ntohs(nla_get_be16(tb[NFTA_QUEUE_FLAGS]));
		if (priv->flags & ~NFT_QUEUE_FLAG_MASK)
			return -EINVAL;
		if (priv->flags & NFT_QUEUE_FLAG_CPU_FANOUT)
			return -EOPNOTSUPP;
	}

	return 0;
}

static int nft_queue_dump(struct sk_buff *skb,
			  const struct nft_expr *expr, bool reset)
{
	const struct nft_queue *priv = nft_expr_priv(expr);

	if (nla_put_be16(skb, NFTA_QUEUE_NUM, htons(priv->queuenum)) ||
	    nla_put_be16(skb, NFTA_QUEUE_TOTAL, htons(priv->queues_total)) ||
	    nla_put_be16(skb, NFTA_QUEUE_FLAGS, htons(priv->flags)))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}

static int
nft_queue_sreg_dump(struct sk_buff *skb,
		    const struct nft_expr *expr, bool reset)
{
	const struct nft_queue *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_QUEUE_SREG_QNUM, priv->sreg_qnum) ||
	    nla_put_be16(skb, NFTA_QUEUE_FLAGS, htons(priv->flags)))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}

static struct nft_expr_type nft_queue_type;
static const struct nft_expr_ops nft_queue_ops = {
	.type		= &nft_queue_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_queue)),
	.eval		= nft_queue_eval,
	.init		= nft_queue_init,
	.dump		= nft_queue_dump,
	.validate	= nft_queue_validate,
	.reduce		= NFT_REDUCE_READONLY,
};

static const struct nft_expr_ops nft_queue_sreg_ops = {
	.type		= &nft_queue_type,
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_queue)),
	.eval		= nft_queue_sreg_eval,
	.init		= nft_queue_sreg_init,
	.dump		= nft_queue_sreg_dump,
	.validate	= nft_queue_validate,
	.reduce		= NFT_REDUCE_READONLY,
};

static const struct nft_expr_ops *
nft_queue_select_ops(const struct nft_ctx *ctx,
		     const struct nlattr * const tb[])
{
	if (tb[NFTA_QUEUE_NUM] && tb[NFTA_QUEUE_SREG_QNUM])
		return ERR_PTR(-EINVAL);

	init_hashrandom(&jhash_initval);

	if (tb[NFTA_QUEUE_NUM])
		return &nft_queue_ops;

	if (tb[NFTA_QUEUE_SREG_QNUM])
		return &nft_queue_sreg_ops;

	return ERR_PTR(-EINVAL);
}

static struct nft_expr_type nft_queue_type __read_mostly = {
	.name		= "queue",
	.select_ops	= nft_queue_select_ops,
	.policy		= nft_queue_policy,
	.maxattr	= NFTA_QUEUE_MAX,
	.owner		= THIS_MODULE,
};

static int __init nft_queue_module_init(void)
{
	return nft_register_expr(&nft_queue_type);
}

static void __exit nft_queue_module_exit(void)
{
	nft_unregister_expr(&nft_queue_type);
}

module_init(nft_queue_module_init);
module_exit(nft_queue_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Leblond <eric@regit.org>");
MODULE_ALIAS_NFT_EXPR("queue");
MODULE_DESCRIPTION("Netfilter nftables queue module");
