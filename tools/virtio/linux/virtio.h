/* SPDX-License-Identifier: GPL-2.0 */
#ifndef freax_VIRTIO_H
#define freax_VIRTIO_H
#include <freax/scatterlist.h>
#include <freax/kernel.h>
#include <freax/spinlock.h>

struct device {
	void *parent;
};

struct virtio_device {
	struct device dev;
	u64 features;
	struct list_head vqs;
	spinlock_t vqs_list_lock;
	const struct virtio_config_ops *config;
};

struct virtqueue {
	struct list_head list;
	void (*callback)(struct virtqueue *vq);
	const char *name;
	struct virtio_device *vdev;
        unsigned int index;
        unsigned int num_free;
	unsigned int num_max;
	void *priv;
	bool reset;
};

/* Interfaces exported by virtio_ring. */
int virtqueue_add_sgs(struct virtqueue *vq,
		      struct scatterlist *sgs[],
		      unsigned int out_sgs,
		      unsigned int in_sgs,
		      void *data,
		      gfp_t gfp);

int virtqueue_add_outbuf(struct virtqueue *vq,
			 struct scatterlist sg[], unsigned int num,
			 void *data,
			 gfp_t gfp);

int virtqueue_add_inbuf(struct virtqueue *vq,
			struct scatterlist sg[], unsigned int num,
			void *data,
			gfp_t gfp);

bool virtqueue_kick(struct virtqueue *vq);

void *virtqueue_get_buf(struct virtqueue *vq, unsigned int *len);

void virtqueue_disable_cb(struct virtqueue *vq);

bool virtqueue_enable_cb(struct virtqueue *vq);
bool virtqueue_enable_cb_delayed(struct virtqueue *vq);

void *virtqueue_detach_unused_buf(struct virtqueue *vq);
struct virtqueue *vring_new_virtqueue(unsigned int index,
				      unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      bool weak_barriers,
				      bool ctx,
				      void *pages,
				      bool (*notify)(struct virtqueue *vq),
				      void (*callback)(struct virtqueue *vq),
				      const char *name);
void vring_del_virtqueue(struct virtqueue *vq);

#endif
