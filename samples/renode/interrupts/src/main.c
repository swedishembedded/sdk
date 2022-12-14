// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 *
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 *
 * This example shows how to send interrupt notifications back to the firmware.
 **/

#include <float.h>

#include <instruments/keypad.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

static volatile struct keypad_instrument *vdev = ((volatile struct keypad_instrument *)0x70000000);

#define IRQ_PIN 8

struct application {
	struct gpio_callback irq_cb;
};

static struct application app;

static void irq_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	(void)dev;
	(void)cb;
	(void)pins;
	// read interrupt flags
	uint32_t intf = vdev->keys_changed;

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

	gpio_init_callback(&app.irq_cb, irq_handler, BIT(IRQ_PIN));
	if (gpio_add_callback(dev, &app.irq_cb) != 0) {
		printk("Error setting callback for GPIO\n");
		return;
	}

	while (1) {
		k_sleep(K_MSEC(10));
	}
}
