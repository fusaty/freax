// SPDX-License-Identifier: GPL-2.0
#include <freax/compiler.h>
#include <freax/bitmap.h>
#include <perf/cpumap.h>
#include <internal/cpumap.h>
#include "tests.h"
#include "debug.h"

#define NBITS 100

static unsigned long *get_bitmap(const char *str, int nbits)
{
	struct perf_cpu_map *map = perf_cpu_map__new(str);
	unsigned long *bm = NULL;
	int i;

	bm = bitmap_zalloc(nbits);

	if (map && bm) {
		for (i = 0; i < perf_cpu_map__nr(map); i++)
			__set_bit(perf_cpu_map__cpu(map, i).cpu, bm);
	}

	if (map)
		perf_cpu_map__put(map);
	return bm;
}

static int test_bitmap(const char *str)
{
	unsigned long *bm = get_bitmap(str, NBITS);
	char buf[100];
	int ret;

	bitmap_scnprintf(bm, NBITS, buf, sizeof(buf));
	pr_debug("bitmap: %s\n", buf);

	ret = !strcmp(buf, str);
	free(bm);
	return ret;
}

static int test__bitmap_print(struct test_suite *test __maybe_unused, int subtest __maybe_unused)
{
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1,5"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1,3,5,7,9,11,13,15,17,19,21-40"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("2-5"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1,3-6,8-10,24,35-37"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1,3-6,8-10,24,35-37"));
	TEST_ASSERT_VAL("failed to convert map", test_bitmap("1-10,12-20,22-30,32-40"));
	return 0;
}

DEFINE_SUITE("Print bitmap", bitmap_print);
