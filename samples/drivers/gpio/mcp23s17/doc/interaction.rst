.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Interacting with the sample
---------------------------

The simulation for this sample creates 16 virtual buttons that are connected to
the gpio pins. The sample itself registers a callback that is called whenever
one of these pins changes state.

If you look in the custom_board.repl file in the sample folder, you'll see how
this is defined:

.. code-block::

	gpioPortB:
		7 -> gpioExpander@16

	A0: Miscellaneous.Button @ gpioExpander
		-> gpioExpander@0

	...

	B7: Miscellaneous.Button @ gpioExpander
		-> gpioExpander@15

	gpioExpander: GPIOPort.MCP23S17 @ spi1
		INTA -> gpioPortB@8
		INTB -> gpioPortB@9

This means you can access these pins and call functions "Press" and "Release" on
them through the renode console:

.. code-block::

	sysbus.spi1.gpioExpander.A0 Press
	sysbus.spi1.gpioExpander.A0 Release

Tip: Use tab for completion.

You can see the result of doing this printed into console.

The simulation also exposes the uart as a pseudoterminal so you can connect to
it using normal terminal tools:

.. code-block::

	picocom build/usart2

The sample also includes gpio shell commands which can be used to inspect GPIO
state and configure the GPIO pins. You can access these commands by connecting
to the USART port.

.. code-block::

	 usart:~$ gpio conf GPIO_E0 1 out
	 usart:~$ gpio conf GPIO_E0 2 in
	 usart:~$ gpio get GPIO_E0 1
	 usart:~$ gpio set GPIO_E0 1 1

You can try a combination of the commands by pressing the virtual buttons and
reading the results using gpio commands:

.. code-block::

	 usart:~$ gpio get GPIO_E0 1
	 (machine-0) sysbus.spi1.gpioExpander.A1 Press
	 usart:~$ gpio get GPIO_E0 1
