Recap
=====

We have covered following commands in this guide:

.. code-block::

	docker pull swedishembedded/develop:latest
	docker run -ti swedishembedded/develop:latest
	west build -b custom_board samples/drivers/gpio/mcp23s17
	west build -b custom_board samples/drivers/gpio/mcp23s17 -t simulate
	picocom build/usart2
	west build -b custom_board samples/drivers/gpio/mcp23s17 \
		-t simulate-debugserver
	west build -b stm32f429i_disc1 ../zephyr/samples/basic/blinky \
		-t flash
	west boards

Hopefully this is enough to get you accustomed with the typical way of cross
compiling applications.
