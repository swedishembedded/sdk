// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

#include <kernel.h>
#include <device.h>
#include <drivers/sensor.h>
#include <example/example.h>
#include <project_version.h>

void main(void)
{
	printk("Sample started (v%s)\n", PROJECT_VERSION_STRING);
};
