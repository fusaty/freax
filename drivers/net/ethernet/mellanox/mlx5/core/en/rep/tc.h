/* SPDX-License-Identifier: GPL-2.0 OR freax-OpenIB */
/* Copyright (c) 2020 Mellanox Technologies. */

#ifndef __MLX5_EN_REP_TC_H__
#define __MLX5_EN_REP_TC_H__

#include <freax/skbuff.h>
#include "en_tc.h"
#include "en_rep.h"

#if IS_ENABLED(CONFIG_MLX5_CLS_ACT)

int mlx5e_rep_tc_init(struct mlx5e_rep_priv *rpriv);
void mlx5e_rep_tc_cleanup(struct mlx5e_rep_priv *rpriv);

int mlx5e_rep_tc_netdevice_event_register(struct mlx5e_rep_priv *rpriv);
void mlx5e_rep_tc_netdevice_event_unregister(struct mlx5e_rep_priv *rpriv);

void mlx5e_rep_tc_enable(struct mlx5e_priv *priv);
void mlx5e_rep_tc_disable(struct mlx5e_priv *priv);

int mlx5e_rep_tc_event_port_affinity(struct mlx5e_priv *priv);

void mlx5e_rep_update_flows(struct mlx5e_priv *priv,
			    struct mlx5e_encap_entry *e,
			    bool neigh_connected,
			    unsigned char ha[ETH_ALEN]);

int mlx5e_rep_encap_entry_attach(struct mlx5e_priv *priv,
				 struct mlx5e_encap_entry *e,
				 struct mlx5e_neigh *m_neigh,
				 struct net_device *neigh_dev);
void mlx5e_rep_encap_entry_detach(struct mlx5e_priv *priv,
				  struct mlx5e_encap_entry *e);

int mlx5e_rep_setup_tc(struct net_device *dev, enum tc_setup_type type,
		       void *type_data);

void mlx5e_rep_tc_receive(struct mlx5_cqe64 *cqe, struct mlx5e_rq *rq,
			  struct sk_buff *skb);

#else /* CONFIG_MLX5_CLS_ACT */

struct mlx5e_rep_priv;
static inline int
mlx5e_rep_tc_init(struct mlx5e_rep_priv *rpriv) { return 0; }
static inline void
mlx5e_rep_tc_cleanup(struct mlx5e_rep_priv *rpriv) {}

static inline int
mlx5e_rep_tc_netdevice_event_register(struct mlx5e_rep_priv *rpriv) { return 0; }
static inline void
mlx5e_rep_tc_netdevice_event_unregister(struct mlx5e_rep_priv *rpriv) {}

static inline void
mlx5e_rep_tc_enable(struct mlx5e_priv *priv) {}
static inline void
mlx5e_rep_tc_disable(struct mlx5e_priv *priv) {}

static inline int
mlx5e_rep_tc_event_port_affinity(struct mlx5e_priv *priv) { return NOTIFY_DONE; }

static inline int
mlx5e_rep_setup_tc(struct net_device *dev, enum tc_setup_type type,
		   void *type_data) { return -EOPNOTSUPP; }

static inline void
mlx5e_rep_tc_receive(struct mlx5_cqe64 *cqe, struct mlx5e_rq *rq,
		     struct sk_buff *skb) { napi_gro_receive(rq->cq.napi, skb); }

#endif /* CONFIG_MLX5_CLS_ACT */

#endif /* __MLX5_EN_REP_TC_H__ */
