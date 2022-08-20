.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Flashing on real hardware
=========================

You can also flash the application on the real nucleo board. For this you will
need to expose USB to the docker image so you can connect to your JTAG adapter.
This can be done by running the docker image in privileged mode and mounting usb
devices. On linux this can be done like this:

.. code-block:: shell

	docker run -t -i --privileged -v /dev/bus/usb:/dev/bus/usb \
		swedishembedded/develop:latest

Now you should be able to flash a sample application like this:

.. code-block:: shell

	west build -b stm32f429i_disc1 ../zephyr/samples/basic/blinky \
		-t flash

For a list of boards you can execute:

.. code-block:: shell

	west boards

Note that not all applications support all of the boards. Specially not ones
that rely on external peripherals. This means that you may not be able to build
them unless the app is configured to support the target board.
