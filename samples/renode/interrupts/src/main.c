// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 *
 * This example shows how to send interrupt notifications back to the firmware.
 **/

#include <kernel.h>
#include <drivers/gpio.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "../peripheral/virtual_device.h"

static volatile struct virtual_device *vdev = ((volatile struct virtual_device *)0x70000000);

#define IRQ_PIN 8

struct application {
	struct gpio_callback irq_cb;
};

static struct application app;

static void _irq_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	// read interrupt flags
	uint32_t intf = vdev->INTF;

	printk("IRQ triggered (INTF: %08x)\n", intf);
}

void main(void)
{
	printk("Renode interrupt handling sample\n");

	const struct device *dev = device_get_binding("GPIOB");

	if (!dev) {
		printk("Could not get GPIO\n");
		return;
	}

	if (gpio_pin_configure(dev, IRQ_PIN, GPIO_INPUT) != 0) {
		printk("Error initializing INTA gpio\n");
		return;
	}
	if (gpio_pin_interrupt_configure(dev, IRQ_PIN, GPIO_INT_EDGE_TO_ACTIVE) != 0) {
		printk("Error configuring interrupt on INTA gpio\n");
		return;
	}

	gpio_init_callback(&app.irq_cb, _irq_handler, BIT(IRQ_PIN));
	if (gpio_add_callback(dev, &app.irq_cb) != 0) {
		printk("Error setting callback for GPIO\n");
		return;
	}

	while (1) {
		k_sleep(K_MSEC(10));
	}
}
