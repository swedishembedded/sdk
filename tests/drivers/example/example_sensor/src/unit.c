// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include "mock_kernel.h"
#include "mock_gpio.h"
#include "mock_spi.h"

#include <unity.h>
#include <example/example.h>
#include <zephyr/device.h>
#include <cmock_unit.h>
#include "../../../../../drivers/example/example_sensor.c"

struct example_sensor_config conf;
struct example_sensor ex = { .config = &conf };
struct device dev = { .data = &ex, .config = &conf };

void test_example_sensor_spi_read_reg_should_return_bytes_read_on_success(void)
{
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(0, _example_sensor_spi_read_reg(&ex, 0, NULL, 0));
}

void test_example_sensor_spi_read_reg_should_return_eio_on_spi_fail(void)
{
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _example_sensor_spi_read_reg(&ex, 0, NULL, 0));
}

void test_example_sensor_irq_work_should_return_without_giving_sem_on_read_fail(void)
{
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	_example_sensor_irq_work(&ex.work);
}

void test_example_sensor_irq_work_should_give_sem_on_success(void)
{
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_sem_give_ExpectAnyArgs();
	_example_sensor_irq_work(&ex.work);
}

void test_example_sensor_gpio_callback_should_submit_irq_work(void)
{
	__wrap_k_work_submit_ExpectAndReturn(&ex.work, 0);
	_example_sensor_gpio_callback(NULL, &ex.gpio_cb, 0);
}

void test_example_sensor_init_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, _example_sensor_init(NULL));
}

void test_example_sensor_init_should_return_eio_on_io_operation_fail(void)
{
	__wrap_k_work_init_ExpectAnyArgs();
	__wrap_k_sem_init_ExpectAnyArgsAndReturn(-1);
	TEST_ASSERT_EQUAL(-EIO, _example_sensor_init(&dev));

	__wrap_k_work_init_ExpectAnyArgs();
	__wrap_k_sem_init_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_pin_configure_dt_ExpectAnyArgsAndReturn(-1);
	TEST_ASSERT_EQUAL(-EIO, _example_sensor_init(&dev));

	__wrap_k_work_init_ExpectAnyArgs();
	__wrap_k_sem_init_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_pin_configure_dt_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_init_callback_Expect(&ex.gpio_cb, _example_sensor_gpio_callback,
					 BIT(conf.gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.gpio.port, &ex.gpio_cb, -1);
	TEST_ASSERT_EQUAL(-EIO, _example_sensor_init(&dev));

	__wrap_k_work_init_ExpectAnyArgs();
	__wrap_k_sem_init_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_pin_configure_dt_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_init_callback_Expect(&ex.gpio_cb, _example_sensor_gpio_callback,
					 BIT(conf.gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.gpio.port, &ex.gpio_cb, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAnyArgsAndReturn(-1);
	TEST_ASSERT_EQUAL(-EIO, _example_sensor_init(&dev));
}

void test_example_sensor_init_should_initialize_driver(void)
{
	__wrap_k_work_init_ExpectAnyArgs();
	__wrap_k_sem_init_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_pin_configure_dt_ExpectAnyArgsAndReturn(0);
	__wrap_gpio_init_callback_Expect(&ex.gpio_cb, _example_sensor_gpio_callback,
					 BIT(conf.gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.gpio.port, &ex.gpio_cb, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAnyArgsAndReturn(0);
	TEST_ASSERT_EQUAL(0, _example_sensor_init(&dev));
}

void test_example_sensor_sample_fetch_should_reset_and_take_semaphore(void)
{
	__wrap_k_sem_reset_Expect(&ex.sample_ready);
	__wrap_k_sem_take_ExpectAndReturn(&ex.sample_ready, K_FOREVER, 0);
	TEST_ASSERT_EQUAL(0, _example_sensor_sample_fetch(&dev, SENSOR_CHAN_AMBIENT_TEMP));
}

void test_example_sensor_sample_channel_get_should_return_value(void)
{
	struct sensor_value val;

	ex.temperature = 121;
	TEST_ASSERT_EQUAL(0, _example_sensor_channel_get(&dev, SENSOR_CHAN_AMBIENT_TEMP, &val));
	TEST_ASSERT_EQUAL(12, val.val1);
	TEST_ASSERT_EQUAL(1, val.val2);
}

void test_example_sensor_sample_channel_get_should_return_enotsup_for_wrong_channel(void)
{
	TEST_ASSERT_EQUAL(-ENOTSUP, _example_sensor_channel_get(&dev, -1, NULL));
}
