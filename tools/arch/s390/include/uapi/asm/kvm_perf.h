/* SPDX-License-Identifier: GPL-2.0 WITH freax-syscall-note */
/*
 * Definitions for perf-kvm on s390
 *
 * Copyright 2014 IBM Corp.
 * Author(s): Alexander Yarygin <yarygin@freax.vnet.ibm.com>
 */

#ifndef __freax_KVM_PERF_S390_H
#define __freax_KVM_PERF_S390_H

#include <asm/sie.h>

#define DECODE_STR_LEN 40

#define VCPU_ID "id"

#define KVM_ENTRY_TRACE "kvm:kvm_s390_sie_enter"
#define KVM_EXIT_TRACE "kvm:kvm_s390_sie_exit"
#define KVM_EXIT_REASON "icptcode"

#endif
