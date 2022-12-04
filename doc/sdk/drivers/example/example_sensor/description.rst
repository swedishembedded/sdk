.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Functional Description
**********************

This driver implements the sensor interface which allows you to access the
simulated device over SPI.

This driver also has a simulation counterpart which you can enable as follows:

	exampleSensor: Sensors.ExampleSensor @ spi1
	   Interrupt -> gpioPortA@4

See "renode/drivers/example/ExampleSensor.cs" for details.

Also we need to wire the CS pin to sensor GPIO 0:

	gpioPortB:
	   6 -> exampleSensor@0

Then from renode you can run:

	sysbus.spi1.exampleSensor Temperature 25

This will simulate a new reading being ready and raise the sensor interrupt
which will be detected by the Zephyr driver and new value will be retrieved.
