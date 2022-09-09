// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <example/example.h>

#include <zephyr/kernel.h>

void main(void)
{
	struct example_object _ex;

	if (example_object_init(&_ex) != 0) {
		printk("Could not initialize example object\n");
	}
};
