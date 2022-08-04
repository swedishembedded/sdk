/*
 * Copyright (c) 2022 Martin Schröder Consulting
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <unity.h>
#include <example/example.h>
#include <device.h>
#include <mock_kernel.h>
#include <mock_gpio.h>
#include <mock_spi.h>

#include <cmock_unit.h>
#include "../../../../../drivers/gpio/mcp23s17.c"

struct device_state inta_state, intb_state;
struct device inta_port = {
	.state = &inta_state,
};
struct device intb_port = {
	.state = &intb_state,
};
struct mcp23s17_config conf = {
	.inta_gpio = {
		.port = &inta_port,
	},
	.intb_gpio = {
		.port = &intb_port,
	},
};
struct mcp23s17 data;
struct device dev = { .data = &data, .config = &conf };

void setUp(void)
{
	data.dev = &dev;
	inta_state.initialized = true;
	intb_state.initialized = true;
	conf.inta_gpio.port = &inta_port;
	conf.intb_gpio.port = &intb_port;
	__wrap_spi_transceive_dt_AddCallback(NULL);
}

void test_mcp23s17_read_regs_should_return_einval_on_invalid_params(void)
{
	uint8_t val = 0;

	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_read_regs(&dev, 0, NULL, sizeof(val)));
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_read_regs(NULL, 0, &val, sizeof(val)));
}

void test_mcp23s17_read_regs_should_return_eio_on_spi_fail(void)
{
	uint8_t val = 0;

	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_read_regs(&dev, 0, &val, sizeof(val)));
}

static int _spi_transceive_dt_clear_rx(const struct spi_dt_spec *spec,
				       const struct spi_buf_set *tx_bufs,
				       const struct spi_buf_set *rx_bufs, int call_no)
{
	// clear the whole rx buffer
	memset(rx_bufs->buffers[1].buf, 0, rx_bufs->buffers[1].len);
	return 0;
}

void test_mcp23s17_read_regs_should_return_zero_on_success(void)
{
	uint8_t val = 123;

	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_clear_rx);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_read_regs(&dev, 0, &val, sizeof(val)));
	TEST_ASSERT_EQUAL(0, val);
}

void test_mcp23s17_write_regs_should_return_eio_on_spi_fail(void)
{
	uint8_t val = 0;

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_write_regs(&dev, 0, &val, sizeof(val)));
}

void test_mcp23s17_write_regs_should_return_zero_on_success(void)
{
	uint8_t val = 0;

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_write_regs(&dev, 0, &val, sizeof(val)));
}

void test_mcp23s17_pin_configure_direction_should_return_eio_on_spi_fail(void)
{
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure_direction(&dev, 0, 0));

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure_direction(&dev, 0, 0));
}

void test_mcp23s17_pin_configure_direction_should_return_zero_on_success(void)
{
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure_direction(&dev, 0, 0));

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(
		0, _mcp23s17_pin_configure_direction(&dev, 0, GPIO_OUTPUT | GPIO_OUTPUT_INIT_HIGH));

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure_direction(&dev, 0,
							       GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOW));

	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure_direction(
				     &dev, 0, GPIO_OUTPUT | GPIO_OUTPUT_INIT_LOGICAL));
}

void test_mcp23s17_pin_configure_pullup_should_return_enotsup_on_pulldown(void)
{
	TEST_ASSERT_EQUAL(-ENOTSUP, _mcp23s17_pin_configure_pullup(&dev, 0, GPIO_PULL_DOWN));
}

void test_mcp23s17_pin_configure_pullup_should_return_zero_on_success(void)
{
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure_pullup(&dev, 0, 0));
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure_pullup(&dev, 0, GPIO_PULL_UP));
}

void test_mcp23s17_pin_configure_pullup_should_return_eio_on_spi_fail(void)
{
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure_pullup(&dev, 0, 0));
}

void test_mcp23s17_pin_configure_should_return_ewouldblock_if_in_isr(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(1);
	TEST_ASSERT_EQUAL(-EWOULDBLOCK, _mcp23s17_pin_configure(&dev, 0, 0));
}

void test_mcp23s17_pin_configure_should_return_enotsup_if_open_drain(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-ENOTSUP, _mcp23s17_pin_configure(&dev, 0, GPIO_OPEN_DRAIN));
}

void test_mcp23s17_pin_configure_should_return_eio_if_spi_fails(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure(&dev, 0, 0));

	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure(&dev, 0, 0));
}

void test_mcp23s17_pin_configure_should_return_zero_on_success(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_configure(&dev, 0, 0));

	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_configure(&dev, 0, 0));
}

void test_mcp23s17_port_get_raw_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_port_get_raw(&dev, NULL));
}

void test_mcp23s17_port_get_raw_should_return_ewouldblock_if_in_isr(void)
{
	uint32_t raw = 0;

	__wrap_k_is_in_isr_ExpectAndReturn(1);
	TEST_ASSERT_EQUAL(-EWOULDBLOCK, _mcp23s17_port_get_raw(&dev, &raw));
}

void test_mcp23s17_port_get_raw_should_return_eio_if_spi_fails(void)
{
	uint32_t raw = 0;

	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_port_get_raw(&dev, &raw));
}

void test_mcp23s17_port_get_raw_should_return_zero_on_success(void)
{
	uint32_t raw = 0;

	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_port_get_raw(&dev, &raw));
}

void test_mcp23s17_port_set_masked_raw_should_return_ewouldblock_if_in_isr(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(1);
	TEST_ASSERT_EQUAL(-EWOULDBLOCK, _mcp23s17_port_set_masked_raw(&dev, 0, 0));
}

void test_mcp23s17_port_set_masked_raw_should_return_eio_on_spi_fail(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_port_set_masked_raw(&dev, 0, 0));
}

void test_mcp23s17_port_set_masked_raw_should_return_zero_on_success(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_port_set_masked_raw(&dev, 0, 0));
}

void test_mcp23s17_port_set_bits_raw_should_return_zero_on_success(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_port_set_bits_raw(&dev, 1));
}

void test_mcp23s17_port_clear_bits_raw_should_return_zero_on_success(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_port_clear_bits_raw(&dev, 1));
}

void test_mcp23s17_port_toggle_bits_should_return_ewouldblock_if_in_isr(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(1);
	TEST_ASSERT_EQUAL(-EWOULDBLOCK, _mcp23s17_port_toggle_bits(&dev, 0));
}

void test_mcp23s17_port_toggle_bits_should_return_eio_on_spi_fail(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_port_toggle_bits(&dev, 1));
}

void test_mcp23s17_port_toggle_bits_should_return_zero_on_success(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_port_toggle_bits(&dev, 1));
}

void test_mcp23s17_pin_interrupt_configure_should_return_enotsup_if_no_inta_dts(void)
{
	conf.inta_gpio.port = NULL;
	conf.intb_gpio.port = NULL;
	TEST_ASSERT_EQUAL(-ENOTSUP, _mcp23s17_pin_interrupt_configure(&dev, 0, GPIO_INT_MODE_EDGE,
								      GPIO_INT_TRIG_LOW));
	TEST_ASSERT_EQUAL(-ENOTSUP, _mcp23s17_pin_interrupt_configure(&dev, 8, GPIO_INT_MODE_EDGE,
								      GPIO_INT_TRIG_LOW));
	conf.inta_gpio.port = &inta_port;
	conf.intb_gpio.port = &intb_port;
}

void test_mcp23s17_pin_interrupt_configure_should_return_enotsup_if_mode_level(void)
{
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-ENOTSUP,
			  _mcp23s17_pin_interrupt_configure(&dev, 0, GPIO_INT_MODE_LEVEL, 0));
}

void test_mcp23s17_pin_interrupt_configure_should_reset_bit_in_gpinten_when_interrupt_disabled(void)
{
	data.regs.GPINTENA = 0xff;
	data.regs.GPINTENB = 0xff;
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 1, GPIO_INT_MODE_DISABLED, 0));
	TEST_ASSERT_EQUAL(0xfd, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0xff, data.regs.GPINTENB);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0,
			  _mcp23s17_pin_interrupt_configure(&dev, 15, GPIO_INT_MODE_DISABLED, 0));
	TEST_ASSERT_EQUAL(0xfd, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x7f, data.regs.GPINTENB);
}

void test_mcp23s17_pin_interrupt_configure_should_work_for_trig_both(void)
{
	data.regs.GPINTENA = 0x00;
	data.regs.GPINTENB = 0x00;
	data.regs.INTCONA = 0xff;
	data.regs.INTCONB = 0xff;
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 1, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_BOTH));
	// second pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x00, data.regs.GPINTENB);
	// interrupt control bit should be 0: compare against previous state
	TEST_ASSERT_EQUAL(0xfd, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0xff, data.regs.INTCONB);
	// we don't care about defval
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 15, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_BOTH));
	// last pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x80, data.regs.GPINTENB);
	// interrupt control bit should be 0: compare against previous state
	TEST_ASSERT_EQUAL(0xfd, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0x7f, data.regs.INTCONB);
}

void test_mcp23s17_pin_interrupt_configure_should_work_for_trig_low(void)
{
	data.regs.GPINTENA = 0x00;
	data.regs.GPINTENB = 0x00;
	data.regs.INTCONA = 0x00;
	data.regs.INTCONB = 0x00;
	data.regs.DEFVALA = 0x00;
	data.regs.DEFVALB = 0x00;
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 1, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_LOW));
	// second pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x00, data.regs.GPINTENB);
	// interrupt control bit should be 0: compare against previous state
	TEST_ASSERT_EQUAL(0x02, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0x00, data.regs.INTCONB);
	// defval should have the bit set so interrupt occurs when bits are different
	TEST_ASSERT_EQUAL(0x02, data.regs.DEFVALA);
	TEST_ASSERT_EQUAL(0x00, data.regs.DEFVALB);

	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 15, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_LOW));
	// last pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x80, data.regs.GPINTENB);
	// interrupt control bit should be 1: compare against defval
	TEST_ASSERT_EQUAL(0x02, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0x80, data.regs.INTCONB);
	// defval should have the bit set so interrupt occurs when bits are different
	TEST_ASSERT_EQUAL(0x02, data.regs.DEFVALA);
	TEST_ASSERT_EQUAL(0x80, data.regs.DEFVALB);
}

void test_mcp23s17_pin_interrupt_configure_should_work_for_trig_high(void)
{
	data.regs.GPINTENA = 0x00;
	data.regs.GPINTENB = 0x00;
	data.regs.INTCONA = 0x00;
	data.regs.INTCONB = 0x00;
	data.regs.DEFVALA = 0xff;
	data.regs.DEFVALB = 0xff;
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 1, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_HIGH));
	// second pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x00, data.regs.GPINTENB);
	// interrupt control bit should be 1: compare against defval
	TEST_ASSERT_EQUAL(0x02, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0x00, data.regs.INTCONB);
	// defval should have the bit set so interrupt occurs when bits are different
	TEST_ASSERT_EQUAL(0xfd, data.regs.DEFVALA);
	TEST_ASSERT_EQUAL(0xff, data.regs.DEFVALB);

	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, 0);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_pin_interrupt_configure(&dev, 15, GPIO_INT_MODE_EDGE,
							       GPIO_INT_TRIG_HIGH));
	// last pin interrupt should be enabled
	TEST_ASSERT_EQUAL(0x02, data.regs.GPINTENA);
	TEST_ASSERT_EQUAL(0x80, data.regs.GPINTENB);
	// interrupt control bit should be 0: compare against previous state
	TEST_ASSERT_EQUAL(0x02, data.regs.INTCONA);
	TEST_ASSERT_EQUAL(0x80, data.regs.INTCONB);
	// defval should have the bit set so interrupt occurs when bits are different
	TEST_ASSERT_EQUAL(0xfd, data.regs.DEFVALA);
	TEST_ASSERT_EQUAL(0x7f, data.regs.DEFVALB);
}

void test_mcp23s17_pin_interrupt_configure_should_return_eio_on_io_fail(void)
{
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_write_dt_ExpectAndReturn(&conf.spi, NULL, -1);
	__wrap_spi_write_dt_IgnoreArg_tx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_pin_interrupt_configure(&dev, 15, GPIO_INT_MODE_EDGE,
								  GPIO_INT_TRIG_BOTH));
}

static uint8_t callback_called;
static void _gpio_callback(const struct device *dev, struct gpio_callback *gpio_cb, uint32_t pins)
{
	callback_called = 1;
}

void test_mcp23s17_manage_callback_should_return_einval_when_removing_nonexistent_callback(void)
{
	struct gpio_callback cb = {
		.node = { 0 },
		.handler = _gpio_callback,
		.pin_mask = 0x0001,
	};
	struct gpio_callback cb2 = {
		.node = { 0 },
		.handler = _gpio_callback,
		.pin_mask = 0x0001,
	};

	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, true));
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_manage_callback(&dev, &cb2, false));
	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, false));
}

void test_mcp23s17_manage_callback_should_return_success(void)
{
	struct gpio_callback cb = {
		.node = { 0 },
		.handler = _gpio_callback,
		.pin_mask = 0x0001,
	};

	callback_called = 0;
	// functionality tested in integration/system test
	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, true));
	// call it second time
	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, true));
	gpio_fire_callbacks(&data.cb, &dev, 0xffff);
	gpio_fire_callbacks(&data.cb, &dev, 0xaaaa);
	gpio_fire_callbacks(&data.cb, &dev, 0xbbbb);
	gpio_fire_callbacks(&data.cb, &dev, 0);
	// should still only be called once
	TEST_ASSERT_EQUAL(1, callback_called);
	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, false));
	// call it second time
	TEST_ASSERT_EQUAL(0, _mcp23s17_manage_callback(&dev, &cb, false));
}

void test_mcp23s17_get_pending_int_should_return_ewouldblock_if_in_isr(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(1);
	TEST_ASSERT_EQUAL(-EWOULDBLOCK, _mcp23s17_get_pending_int(&dev));
}

void test_mcp23s17_get_pending_int_should_return_zero_on_io_fail(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_get_pending_int(&dev));
}

static int _spi_transceive_dt_get_intf(const struct spi_dt_spec *spec,
				       const struct spi_buf_set *tx_bufs,
				       const struct spi_buf_set *rx_bufs, int call_no)
{
	memset(rx_bufs->buffers[1].buf, 0, rx_bufs->buffers[1].len);
	// default values for first two registers
	((uint8_t *)rx_bufs->buffers[1].buf)[0] = data.regs.INTFA;
	((uint8_t *)rx_bufs->buffers[1].buf)[1] = data.regs.INTFB;
	return 0;
}

void test_mcp23s17_get_pending_int_should_return_intf_value(void)
{
	__wrap_k_is_in_isr_ExpectAndReturn(0);
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_get_intf);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	data.regs.INTFB = 0x34;
	data.regs.INTFA = 0x12;
	TEST_ASSERT_EQUAL(0x3412, _mcp23s17_get_pending_int(&dev));
}

void test_mcp23s17_irq_work_should_read_intf_on_success(void)
{
	// this is just a run through on success
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_get_intf);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	data.regs.INTFB = 0xff;
	data.regs.INTFA = 0xff;
	_mcp23s17_irq_work(&data.irq_work);
}

void test_mcp23s17_irq_work_should_read_intf_on_io_fail(void)
{
	// this is just a run through on success
	__wrap_k_mutex_lock_ExpectAndReturn(&data.lock, K_FOREVER, 0);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_k_mutex_unlock_ExpectAndReturn(&data.lock, 0);
	data.regs.INTFB = 0xff;
	data.regs.INTFA = 0xff;
	_mcp23s17_irq_work(&data.irq_work);
}

void test_mcp23s17_irqa_callback_should_submit_work(void)
{
	__wrap_k_work_submit_ExpectAndReturn(&data.irq_work, 0);
	_mcp23s17_irqa_callback(&dev, &data.inta_cb, 1);
}

void test_mcp23s17_irqb_callback_should_submit_work(void)
{
	__wrap_k_work_submit_ExpectAndReturn(&data.irq_work, 0);
	_mcp23s17_irqb_callback(&dev, &data.intb_cb, 1);
}

void test_mcp23s17_init_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_init(NULL));
}

static int _spi_transceive_dt_read_regs(const struct spi_dt_spec *spec,
					const struct spi_buf_set *tx_bufs,
					const struct spi_buf_set *rx_bufs, int call_no)
{
	memset(rx_bufs->buffers[1].buf, 0, rx_bufs->buffers[1].len);
	// default values for first two registers
	((uint8_t *)rx_bufs->buffers[1].buf)[0] = 0xff;
	((uint8_t *)rx_bufs->buffers[1].buf)[1] = 0xff;
	return 0;
}

static int _spi_transceive_dt_read_regs_bad_value1(const struct spi_dt_spec *spec,
						   const struct spi_buf_set *tx_bufs,
						   const struct spi_buf_set *rx_bufs, int call_no)
{
	memset(rx_bufs->buffers[1].buf, 0, rx_bufs->buffers[1].len);
	// default values for first two registers
	((uint8_t *)rx_bufs->buffers[1].buf)[0] = 0xff;
	((uint8_t *)rx_bufs->buffers[1].buf)[1] = 0x00;
	return 0;
}

static int _spi_transceive_dt_read_regs_bad_value2(const struct spi_dt_spec *spec,
						   const struct spi_buf_set *tx_bufs,
						   const struct spi_buf_set *rx_bufs, int call_no)
{
	memset(rx_bufs->buffers[1].buf, 0, rx_bufs->buffers[1].len);
	// default values for first two registers
	((uint8_t *)rx_bufs->buffers[1].buf)[0] = 0x00;
	((uint8_t *)rx_bufs->buffers[1].buf)[1] = 0xff;
	return 0;
}

void test_mcp23s17_init_should_return_einval_on_bad_reg_values_on_chip(void)
{
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs_bad_value1);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_init(&dev));
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs_bad_value2);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(-EINVAL, _mcp23s17_init(&dev));
}

void test_mcp23s17_init_should_return_eio_on_initial_read_fail(void)
{
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, -1);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));
}

void test_mcp23s17_init_should_return_eio_when_io_fails(void)
{
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));
}

void test_mcp23s17_init_should_return_zero_on_success(void)
{
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.inta_cb, _mcp23s17_irqa_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.inta_gpio.port, &data.inta_cb, 0);
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.intb_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.intb_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.intb_cb, _mcp23s17_irqb_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.intb_gpio.port, &data.intb_cb, 0);
	TEST_ASSERT_EQUAL(0, _mcp23s17_init(&dev));
}

void test_mcp23s17_init_should_return_zero_when_interrupts_not_supplied(void)
{
	inta_state.initialized = false;
	intb_state.initialized = false;
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	TEST_ASSERT_EQUAL(0, _mcp23s17_init(&dev));
}

void test_mcp23s17_init_should_return_eio_on_io_fail(void)
{
	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));

	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));

	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.inta_cb, _mcp23s17_irqa_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.inta_gpio.port, &data.inta_cb, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));

	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.inta_cb, _mcp23s17_irqa_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.inta_gpio.port, &data.inta_cb, 0);
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.intb_gpio, GPIO_INPUT, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));

	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.inta_cb, _mcp23s17_irqa_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.inta_gpio.port, &data.inta_cb, 0);
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.intb_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.intb_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));

	__wrap_k_mutex_init_ExpectAndReturn(&data.lock, 0);
	__wrap_k_work_init_Expect(&data.irq_work, _mcp23s17_irq_work);
	__wrap_spi_transceive_dt_AddCallback(_spi_transceive_dt_read_regs);
	__wrap_spi_transceive_dt_ExpectAndReturn(&conf.spi, NULL, NULL, 0);
	__wrap_spi_transceive_dt_IgnoreArg_tx_bufs();
	__wrap_spi_transceive_dt_IgnoreArg_rx_bufs();
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.inta_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.inta_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.inta_cb, _mcp23s17_irqa_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.inta_gpio.port, &data.inta_cb, 0);
	__wrap_gpio_pin_configure_dt_ExpectAndReturn(&conf.intb_gpio, GPIO_INPUT, 0);
	__wrap_gpio_pin_interrupt_configure_dt_ExpectAndReturn(&conf.intb_gpio,
							       GPIO_INT_EDGE_TO_ACTIVE, 0);
	__wrap_gpio_init_callback_Expect(&data.intb_cb, _mcp23s17_irqb_callback,
					 BIT(conf.inta_gpio.pin));
	__wrap_gpio_add_callback_ExpectAndReturn(conf.intb_gpio.port, &data.intb_cb, -1);
	TEST_ASSERT_EQUAL(-EIO, _mcp23s17_init(&dev));
}
