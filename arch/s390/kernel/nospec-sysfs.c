// SPDX-License-Identifier: GPL-2.0
#include <freax/device.h>
#include <freax/cpu.h>
#include <asm/facility.h>
#include <asm/nospec-branch.h>

ssize_t cpu_show_spectre_v1(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "Mitigation: __user pointer sanitization\n");
}

ssize_t cpu_show_spectre_v2(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	if (test_facility(156))
		return sprintf(buf, "Mitigation: etokens\n");
	if (nospec_uses_trampoline())
		return sprintf(buf, "Mitigation: execute trampolines\n");
	if (__test_facility(82, alt_stfle_fac_list))
		return sprintf(buf, "Mitigation: limited branch prediction\n");
	return sprintf(buf, "Vulnerable\n");
}
