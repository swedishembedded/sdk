// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include "mock_kernel.h"

#include <cmock_unit.h>
#include <zephyr/device.h>
#include <example/example.h>
#include <unity.h>

#include "../../../../../drivers/example/example_driver.c"

void test_example_driver_init_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, example_driver_init(NULL));
}

void test_example_driver_init_should_initialize_driver(void)
{
	struct example_driver ex;
	struct example_driver_config conf;
	struct device dev = { .data = &ex, .config = &conf };

	TEST_ASSERT_EQUAL(0, example_driver_init(&dev));
}
