.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Testing your application
========================

This SDK provides several powerful ways of testing code. These are:

- **Unit testing**: for this CMock and Unity framework are used which allows
  testing individual functions in isolation.
- **Integration testing**: this uses Zephyr infrastructure for simulated drivers
  (ie simulated SPI, I2C etc. devices) and compiles and runs firmware on
  native_posix.
- **System testing**: this uses testbenches and simulation in order to test the
  firmware as a black box.

Unit testing with CMock and Unity
---------------------------------

Unit tests are separate applications that focus on testing only a single C file.
For this, you create a project under "tests" directory and configure it just
like a normal application at first.

Then you add CMake commands invoking CMock mocking functionality in order to
mock calls outside of your file being tested.

.. code-block:: text

	cmake_minimum_required(VERSION 3.20.0)
	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
	project(test)

	# Configure unity and CMock to generate tests from our code
	unity_generate_test_runner(src/unit.c)
	cmock_generate_mocks(${ZEPHYR_BASE}/include/kernel.h .)
	cmock_generate_mocks(${ZEPHYR_BASE}/include/drivers/gpio.h .)
	cmock_generate_mocks(${ZEPHYR_BASE}/include/drivers/spi.h .)

	target_sources(app PRIVATE src/unit.c)

This generates several headers with mocks.

Inside your unit.c file you then define your tests:

.. code-block:: c

	#include <mock_kernel.h>
	#include <mock_gpio.h>
	#include <mock_spi.h>

	#include "../../../../../drivers/gpio/mcp23s17.c"

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

Notice that we include the C file into our unit test. This is fully correct
because it allows us to test everything inside our file being tested (including
static data and accessing private structures - which is exactly what we want. We
only use this for unit tests because unit tests are special in that they are
used to test all of the implementation details.

Notice that CMock tests are very verbose. This is a good thing. The purpose of a
test is to illuminate internal logic of the code. CMock is capable of enforcing
not just tests for the arguments that your code passes to mocked functions but
their ordering as well.

When we write our unit tests this way, we illuminate the sequence of external
calls that our code calls. This way we can detect wrong return paths (for
example when our code forgets to unlock a mutex or continues execution when it
should have returned). This is very powerful.

However, one thing that this kind of testing doesn't do is test whether our code
interfaces with other modules correctly. We can test that it passes specific
parameters to outside methods but we can not check whether the implementation on
the other end understands what we are trying to do (because the implementation
is mocked - so there is no real implementation in a unit test).

Integration testing your code
-----------------------------

To test whether our code interfaces with other modules correctly, we use
integration testing.

.. code-block:: text

	cmake_minimum_required(VERSION 3.20.0)
	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
	project(test)

	unity_generate_test_runner(src/integration.c)
	target_sources(app PRIVATE src/integration.c)

Integration tests are setup like unit tests, but instead of including our file
under test into our test, we enable them in our testcase.yml file.

.. code-block:: text

	tests:
		drivers.gpio.mcp23s17.integration:
			tags: drivers/gpio/mcp23s17.c
			integration_platforms:
				- native_posix

Note that in order to write integration tests for a particular platform (for
example native posix) you need to either implement device emulation through
Zephyr and plug in your emulated device into the device tree so that it can
respond to your driver requests - or you need to run the integration test on
real or simulated platform where you can get reasonable responses from the
device.

In this way, you can test that your device driver is able to communicate through
SPI with the outside world correctly (which is why we call this "integration"
test).
