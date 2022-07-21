/*
 * Copyright (c) 2021 Antmicro <www.antmicro.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <example/example.h>

void main(void)
{
	struct example_object _ex;

	if (example_object_init(&_ex) != 0) {
		printk("Could not initialize example object\n");
	}
};
