.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Usage
*****

Once device instance has been defined in DTS or overlay and you have enabled the
driver in your prj.conf, you can now get a handle to the device in your
application:

.. code-block::

	const struct device *dev = device_get_binding("GPIO_E0");

You can now configure the pin as input or output using the
:c:func:`gpio_pin_configure` function:

.. code-block::

	gpio_pin_configure(dev, pin, GPIO_INPUT);
	gpio_pin_configure(dev, pin, GPIO_OUTPUT);
	gpio_pin_configure(dev, pin, GPIO_OUTPUT | GPIO_PULL_UP);
	gpio_pin_configure(dev, pin, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	gpio_pin_configure(dev, pin, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);

You can combine any of these flags.

It is also possible to configure interrupts on each pin individually by using
the standard :c:func:`gpio_pin_interrupt_configure` function:

.. code-block::

	gpio_pin_interrupt_configure(dev, pin, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure(dev, pin, GPIO_INT_EDGE_TO_INACTIVE);

You can now add a callback for the interrupts by using the standard set of
zephyr functions:

.. code-block::

	gpio_init_callback(&cb, _gpio_callback, pin_mask);
	gpio_add_callback(dev, &cb);
