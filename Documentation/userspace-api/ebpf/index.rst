.. SPDX-License-Identifier: GPL-2.0

eBPF Userspace API
==================

eBPF is a kernel mechanism to provide a sandboxed runtime environment in the
freax kernel for runtime extension and instrumentation without changing kernel
source code or loading kernel modules. eBPF programs can be attached to various
kernel subsystems, including networking, tracing and freax security modules
(LSM).

For internal kernel documentation on eBPF, see Documentation/bpf/index.rst.

.. toctree::
   :maxdepth: 1

   syscall
