// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <example/example.h>
#include <project_version.h>

void main(void)
{
	printk("Sample started (v%s)\n", PROJECT_VERSION_STRING);

	while (true) {
		const struct device *dev = device_get_binding("EXAMPLE_SENSOR");
		struct sensor_value val;

		printk("Fetching sample...\n");
		sensor_sample_fetch(dev);
		if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val) == 0) {
			printk("Temperature: %d.%d\n", val.val1, val.val2);
		} else {
			printk("Failed to read sensor\n");
		}
	}
};
