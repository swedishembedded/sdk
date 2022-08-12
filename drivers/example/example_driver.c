// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

/** Device tree compatible */
#define DT_DRV_COMPAT example_driver

#include <errno.h>
#include <device.h>
#include <logging/log.h>
#include <drivers/example/example_driver.h>

/** Create a logger */
LOG_MODULE_REGISTER(example_driver);

/** Driver local data */
struct example_driver {
	// data goes here
};

/** Driver configuration (from device tree) */
struct example_driver_config {
	/** a custom variable from dts */
	int custom_variable;
};

/** Driver initialization */
static int _example_driver_init(const struct device *dev)
{
	if (!dev)
		return -EINVAL;
	printk("Example driver initialized!\n");
	LOG_INF("Example driver initialized!");
	return 0;
}

/** Macro to instantiate the driver */
#define EXAMPLE_DRIVER_INIT(n)                                                                     \
	static struct example_driver _example_##n;                                                 \
	static const struct example_driver_config _example_config_##n = {                          \
		.custom_variable = DT_INST_PROP(n, custom_variable),                               \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, _example_driver_init, NULL, &_example_##n, &_example_config_##n,  \
			      POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY, NULL)

/** Instantiate the driver */
DT_INST_FOREACH_STATUS_OKAY(EXAMPLE_DRIVER_INIT);
