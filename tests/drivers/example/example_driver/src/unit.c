// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

#include <unity.h>
#include <example/example.h>
#include <device.h>
#include <mock_kernel.h>

#include <cmock_unit.h>
#include "../../../../../drivers/example/example_driver.c"

void test_example_driver_init_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, _example_driver_init(NULL));
}

void test_example_driver_init_should_initialize_driver(void)
{
	struct example_driver ex;
	struct example_driver_config conf;
	struct device dev = { .data = &ex, .config = &conf };

	TEST_ASSERT_EQUAL(0, _example_driver_init(&dev));
}
