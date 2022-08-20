.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Usage
*****

Enable this driver in prj.conf by setting "CONFIG_EXAMPLE_SENSOR=y" in your
application.

.. code-block:: text

	example_sensor {
		compatible = "example-sensor";
		label = "EXAMPLE_SENSOR";
		gpios = <&gpioa 4 GPIO_ACTIVE_LOW>;
		status = "okay";
	};

Then in code you can start a new measurement using:

.. code-block:: text

	const struct device *dev = device_get_binding("EXAMPLE_DRIVER")
	sensor_sample_fetch(dev);
	sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP);
