// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/drivers/gpio.h>
#include <unity.h>

void test_mcp23s17_is_connected(void)
{
	// we can check whether the device works by setting a pin and then getting it
	const struct device *gpio = device_get_binding("mcp23s17@0");

	TEST_ASSERT_NOT_NULL(gpio);
	TEST_ASSERT_EQUAL(0, gpio_pin_get(gpio, 1));
	TEST_ASSERT_EQUAL(0, gpio_pin_set(gpio, 1, 1));
	TEST_ASSERT_EQUAL(1, gpio_pin_get(gpio, 1));
	TEST_ASSERT_EQUAL(0, 0);
}
