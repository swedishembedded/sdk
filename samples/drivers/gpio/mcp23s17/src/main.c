// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <kernel.h>
#include <device.h>
#include <shell/shell.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>
#include <example/example.h>
#include <project_version.h>

struct application {
	struct gpio_callback cb;
};

static struct application app;

static void _irq_gpio(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	for (unsigned int c = 0; c < 8; c++) {
		if (((1 << c) & pins) == 0)
			continue;
		int r = gpio_pin_get(dev, c);

		gpio_pin_set(dev, c + 8, r);
		printk("%s: PIN: %d, Value: %d\n", dev->name, c, r);
	}
}

void main(void)
{
	printk("MCP23S17 Sample (v%s)\n", PROJECT_VERSION_STRING);

	const struct device *dev = device_get_binding("GPIO_E0");

	if (!dev) {
		printk("No device\n");
		return;
	}

	for (unsigned int c = 0; c < 8; c++) {
		if (gpio_pin_configure(dev, c, GPIO_INPUT) != 0) {
			printk("Error initializing INTA gpio\n");
			return;
		}
		if (gpio_pin_interrupt_configure(dev, c, GPIO_INT_EDGE_TO_ACTIVE) != 0) {
			printk("Error configuring interrupt on INTA gpio\n");
			return;
		}
	}

	for (unsigned int c = 8; c < 16; c++) {
		if (gpio_pin_configure(dev, c, GPIO_OUTPUT) != 0) {
			printk("Error initializing INTA gpio\n");
			return;
		}
	}

	gpio_init_callback(&app.cb, _irq_gpio, 0x00ff);
	if (gpio_add_callback(dev, &app.cb) != 0) {
		printk("Error setting callback for GPIO\n");
		return;
	}
};
