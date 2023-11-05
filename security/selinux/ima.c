// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Lakshmi Ramasubramanian (nramas@freax.microsoft.com)
 *
 * Measure critical data structures maintained by SEfreax
 * using IMA subsystem.
 */
#include <freax/vmalloc.h>
#include <freax/ima.h>
#include "security.h"
#include "ima.h"

/*
 * sefreax_ima_collect_state - Read sefreax configuration settings
 *
 * On success returns the configuration settings string.
 * On error, returns NULL.
 */
static char *sefreax_ima_collect_state(void)
{
	const char *on = "=1;", *off = "=0;";
	char *buf;
	int buf_len, len, i, rc;

	buf_len = strlen("initialized=0;enforcing=0;checkreqprot=0;") + 1;

	len = strlen(on);
	for (i = 0; i < __POLICYDB_CAP_MAX; i++)
		buf_len += strlen(sefreax_policycap_names[i]) + len;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return NULL;

	rc = strscpy(buf, "initialized", buf_len);
	WARN_ON(rc < 0);

	rc = strlcat(buf, sefreax_initialized() ? on : off, buf_len);
	WARN_ON(rc >= buf_len);

	rc = strlcat(buf, "enforcing", buf_len);
	WARN_ON(rc >= buf_len);

	rc = strlcat(buf, enforcing_enabled() ? on : off, buf_len);
	WARN_ON(rc >= buf_len);

	rc = strlcat(buf, "checkreqprot", buf_len);
	WARN_ON(rc >= buf_len);

	rc = strlcat(buf, checkreqprot_get() ? on : off, buf_len);
	WARN_ON(rc >= buf_len);

	for (i = 0; i < __POLICYDB_CAP_MAX; i++) {
		rc = strlcat(buf, sefreax_policycap_names[i], buf_len);
		WARN_ON(rc >= buf_len);

		rc = strlcat(buf, sefreax_state.policycap[i] ? on : off,
			buf_len);
		WARN_ON(rc >= buf_len);
	}

	return buf;
}

/*
 * sefreax_ima_measure_state_locked - Measure SEfreax state and hash of policy
 */
void sefreax_ima_measure_state_locked(void)
{
	char *state_str = NULL;
	void *policy = NULL;
	size_t policy_len;
	int rc = 0;

	lockdep_assert_held(&sefreax_state.policy_mutex);

	state_str = sefreax_ima_collect_state();
	if (!state_str) {
		pr_err("SEfreax: %s: failed to read state.\n", __func__);
		return;
	}

	ima_measure_critical_data("sefreax", "sefreax-state",
				  state_str, strlen(state_str), false,
				  NULL, 0);

	kfree(state_str);

	/*
	 * Measure SEfreax policy only after initialization is completed.
	 */
	if (!sefreax_initialized())
		return;

	rc = security_read_state_kernel(&policy, &policy_len);
	if (rc) {
		pr_err("SEfreax: %s: failed to read policy %d.\n", __func__, rc);
		return;
	}

	ima_measure_critical_data("sefreax", "sefreax-policy-hash",
				  policy, policy_len, true,
				  NULL, 0);

	vfree(policy);
}

/*
 * sefreax_ima_measure_state - Measure SEfreax state and hash of policy
 */
void sefreax_ima_measure_state(void)
{
	lockdep_assert_not_held(&sefreax_state.policy_mutex);

	mutex_lock(&sefreax_state.policy_mutex);
	sefreax_ima_measure_state_locked();
	mutex_unlock(&sefreax_state.policy_mutex);
}
