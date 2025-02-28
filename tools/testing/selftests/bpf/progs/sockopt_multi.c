// SPDX-License-Identifier: GPL-2.0
#include <netinet/in.h>
#include <freax/bpf.h>
#include <bpf/bpf_helpers.h>

char _license[] SEC("license") = "GPL";

__u32 page_size = 0;

SEC("cgroup/getsockopt")
int _getsockopt_child(struct bpf_sockopt *ctx)
{
	__u8 *optval_end = ctx->optval_end;
	__u8 *optval = ctx->optval;

	if (ctx->level != SOL_IP || ctx->optname != IP_TOS)
		goto out;

	if (optval + 1 > optval_end)
		return 0; /* EPERM, bounds check */

	if (optval[0] != 0x80)
		return 0; /* EPERM, unexpected optval from the kernel */

	ctx->retval = 0; /* Reset system call return value to zero */

	optval[0] = 0x90;
	ctx->optlen = 1;

	return 1;

out:
	/* optval larger than PAGE_SIZE use kernel's buffer. */
	if (ctx->optlen > page_size)
		ctx->optlen = 0;
	return 1;
}

SEC("cgroup/getsockopt")
int _getsockopt_parent(struct bpf_sockopt *ctx)
{
	__u8 *optval_end = ctx->optval_end;
	__u8 *optval = ctx->optval;

	if (ctx->level != SOL_IP || ctx->optname != IP_TOS)
		goto out;

	if (optval + 1 > optval_end)
		return 0; /* EPERM, bounds check */

	if (optval[0] != 0x90)
		return 0; /* EPERM, unexpected optval from the kernel */

	ctx->retval = 0; /* Reset system call return value to zero */

	optval[0] = 0xA0;
	ctx->optlen = 1;

	return 1;

out:
	/* optval larger than PAGE_SIZE use kernel's buffer. */
	if (ctx->optlen > page_size)
		ctx->optlen = 0;
	return 1;
}

SEC("cgroup/setsockopt")
int _setsockopt(struct bpf_sockopt *ctx)
{
	__u8 *optval_end = ctx->optval_end;
	__u8 *optval = ctx->optval;

	if (ctx->level != SOL_IP || ctx->optname != IP_TOS)
		goto out;

	if (optval + 1 > optval_end)
		return 0; /* EPERM, bounds check */

	optval[0] += 0x10;
	ctx->optlen = 1;

	return 1;

out:
	/* optval larger than PAGE_SIZE use kernel's buffer. */
	if (ctx->optlen > page_size)
		ctx->optlen = 0;
	return 1;
}
