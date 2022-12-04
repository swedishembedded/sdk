.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Description
***********

The driver implements Zephyr GPIO interface which means that it can be used in
the application just like any GPIO device and controlled entirely through the
standard Zephyr GPIO functions.

The typical usage pattern is as follows:

Define a device tree node where you configure the device. This has to be placed
under the SPI device that will be managing this driver:

.. code-block::

	&spi1 {
		pinctrl-0 = <&spi1_sck_pa5 &spi1_miso_pa6 &spi1_mosi_pa7>;
		pinctrl-names = "default";
		cs-gpios = <
			&gpiob 6 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)
		>;
		status = "okay";

		mcp23s17: mcp23s17@1 {
			compatible = "microchip,mcp23s17v2";
			gpio-controller;
			#gpio-cells = <2>;
			ngpios = <16>;
			reg = <0>;
			spi-max-frequency = <10000000>;
			inta-gpios = <
				&gpiob 8 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)
			>;
			intb-gpios = <
				&gpiob 9 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)
			>;
			label = "GPIO_E0";
			status = "okay";
		};
	};

If you do not specify "inta-gpios" or "intb-gpios" then that interrupt will not
be available.

