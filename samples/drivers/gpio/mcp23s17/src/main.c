/*
 * Copyright (c) 2021 Antmicro <www.antmicro.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

static int _cmd_set_handler(const struct shell *shell, size_t argc, char **argv)
{
	const struct device *dev = device_get_binding("GPIO_E0");
	int r = gpio_pin_get(dev, 0);

	shell_print(shell, "PIN: %d\n", r);
	return 0;
}

/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_mcp, SHELL_CMD(set, NULL, "set pin", _cmd_set_handler),
			       SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(mcp, &sub_mcp, "MCP23S17 commands", NULL);

static void _irq_gpio(const struct device *dev, struct gpio_callback *cb, uint32_t pin)
{
	printk("%s: GPA pins: 0x%02x\n", dev->name, pin & 7);
	printk("%s: GPB pins: 0x%02x\n", dev->name, pin >> 8);
}

void main(void)
{
	printk("MCP23S17 Sample (v%s)\n", PROJECT_VERSION_STRING);

	const struct device *dev = device_get_binding("GPIO_E0");

	if (!dev) {
		printk("No device\n");
		return;
	}

	for (unsigned int c = 0; c < 16; c++) {
		if (gpio_pin_configure(dev, c, GPIO_INPUT) != 0) {
			printk("Error initializing INTA gpio\n");
			return;
		}
		if (gpio_pin_interrupt_configure(dev, c, GPIO_INT_EDGE_TO_ACTIVE) != 0) {
			printk("Error configuring interrupt on INTA gpio\n");
			return;
		}
	}

	gpio_init_callback(&app.cb, _irq_gpio, 0xffff);
	if (gpio_add_callback(dev, &app.cb) != 0) {
		printk("Error setting callback for GPIO\n");
		return;
	}
};
