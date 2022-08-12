// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

/** Device tree compatible */
#define DT_DRV_COMPAT example_sensor

#include <errno.h>
#include <device.h>
#include <logging/log.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/sensor.h>

/** Create a logger */
LOG_MODULE_REGISTER(example_sensor);

struct example_sensor_config;

/** Driver local data */
struct example_sensor {
	/** Interrupt callback */
	struct gpio_callback gpio_cb;
	/** Interrupt work */
	struct k_work work;
	/** Sample ready semaphore */
	struct k_sem sample_ready;
	/** Config */
	const struct example_sensor_config *config;
	/** Temperature value */
	int16_t temperature;
};

/** Driver configuration (from device tree) */
struct example_sensor_config {
	/** Interrupt GPIO */
	struct gpio_dt_spec gpio;
	/** SPI spec */
	struct spi_dt_spec spi;
};

static int _example_sensor_spi_read_reg(struct example_sensor *self, uint8_t reg,
					uint8_t *out_buffer, size_t count)
{
	uint8_t cmd_buf[] = { 0x01, reg };

	struct spi_buf tx_buf[] = { { .buf = cmd_buf, .len = sizeof(cmd_buf) },
				    { .buf = NULL, .len = count } };
	const struct spi_buf_set tx = { .buffers = tx_buf, .count = ARRAY_SIZE(tx_buf) };
	struct spi_buf rx_buf[] = { { .buf = NULL, .len = sizeof(cmd_buf) },
				    { .buf = out_buffer, .len = count } };
	const struct spi_buf_set rx = { .buffers = rx_buf, .count = ARRAY_SIZE(rx_buf) };

	if (spi_transceive_dt(&self->config->spi, &tx, &rx) < 0) {
		return -EIO;
	}

	return count;
}

static void _example_sensor_irq_work(struct k_work *item)
{
	struct example_sensor *self = CONTAINER_OF(item, struct example_sensor, work);
	char rx[3] = { 0, 0 };

	if (_example_sensor_spi_read_reg(self, 0xAA, rx, sizeof(rx)) < 0) {
		LOG_ERR("Could not read temperature value");
		return;
	}

	self->temperature = (int16_t)rx[0] << 8 | rx[1];
	k_sem_give(&self->sample_ready);
}

static void _example_sensor_gpio_callback(const struct device *dev, struct gpio_callback *cb,
					  uint32_t pins)
{
	struct example_sensor *self = CONTAINER_OF(cb, struct example_sensor, gpio_cb);

	k_work_submit(&self->work);
}

/** Driver initialization */
static int _example_sensor_init(const struct device *dev)
{
	if (!dev)
		return -EINVAL;

	const struct example_sensor_config *const config = dev->config;
	struct example_sensor *self = dev->data;

	self->config = config;

	k_work_init(&self->work, _example_sensor_irq_work);
	if (k_sem_init(&self->sample_ready, 0, 1) < 0) {
		LOG_ERR("failed to initialize semaphore");
		return -EIO;
	}

	if (gpio_pin_configure_dt(&config->gpio, GPIO_INPUT) < 0) {
		LOG_ERR("failed to configure gpio");
		return -EIO;
	}

	/* Setup callback struct but do not add it yet */
	gpio_init_callback(&self->gpio_cb, _example_sensor_gpio_callback, BIT(config->gpio.pin));
	if (gpio_add_callback(config->gpio.port, &self->gpio_cb) < 0) {
		LOG_ERR("failed to add interrupt callback");
		return -EIO;
	}

	if (gpio_pin_interrupt_configure_dt(&config->gpio, GPIO_INT_EDGE_FALLING) < 0) {
		LOG_ERR("failed to configure gpio interrupt");
		return -EIO;
	}

	return 0;
}

static int _example_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct example_sensor *self = dev->data;

	// wait for new sample to arrive (device will trigger interrupt line
	// when new sample is ready)
	k_sem_reset(&self->sample_ready);
	k_sem_take(&self->sample_ready, K_FOREVER);

	return 0;
}

static int _example_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
				       struct sensor_value *val)
{
	struct example_sensor *self = dev->data;

	if (chan == SENSOR_CHAN_AMBIENT_TEMP) {
		val->val1 = self->temperature / 10;
		val->val2 = self->temperature % 10;
		return 0;
	}

	return -ENOTSUP;
}

static const struct sensor_driver_api example_sensor_api_funcs = {
	.sample_fetch = _example_sensor_sample_fetch,
	.channel_get = _example_sensor_channel_get,
};

#define IIS2DLPC_SPI_OPERATION                                                                     \
	(SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA)

/** Macro to instantiate the driver */
#define EXAMPLE_SENSOR_INIT(n)                                                                     \
	static struct example_sensor _example_##n;                                                 \
	static const struct example_sensor_config _example_config_##n = {                          \
		.gpio = GPIO_DT_SPEC_GET(DT_DRV_INST(n), gpios),                                   \
		.spi = SPI_DT_SPEC_INST_GET(                                                       \
			n, SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA,   \
			0),                                                                        \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, _example_sensor_init, NULL, &_example_##n, &_example_config_##n,  \
			      POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,                       \
			      &example_sensor_api_funcs)

/** Instantiate the driver */
DT_INST_FOREACH_STATUS_OKAY(EXAMPLE_SENSOR_INIT);
