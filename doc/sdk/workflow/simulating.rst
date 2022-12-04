.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Simulating your application
===========================

There are three ways to define a simulation:

- **Inside your application folder**: put the simulation resc file under "board"
  along side your overlay.
- **Inside the main board folder**: put it alongside of board dts file.
- **As a separate testbench**: put it under "testbench" and add Kconfig option
  to enable the testbench.

Regardless of where you put your simulation, the files you add are the same. The
simulation consists of two files:

- **resc**: this is a platform creation script. It runs commands that are
  supported by the simulation monitor. You can extend monitor using python
  (check implementation of the nucleo401re testbench inside this repository).
- **repl**: this is a platform definition file that defines peripherals for a
  machine. If you have multiple machines in your simulation, you would have
  multiple platforms - but only one platform file per machine.

It is the repl file that points to simulation implementation files (which are
written in Python or C#) for every peripheral that is part of a machine.

You can extend repl files easily as well:

.. code-block:: text

	using "platforms/cpus/stm32f4.repl"

	gpioPortB:
			6 -> gpioExpander@16

	A0: Miscellaneous.Button @ gpioExpander
			-> gpioExpander@0

	gpioExpander: GPIOPort.MCP23S17 @ spi1
			INTA -> gpioPortB@8
			INTB -> gpioPortB@9
			8 -> SW0LED@0

	SW0LED: Miscellaneous.LED @ gpioExpander

The corresponding resc file would look like this:

.. code-block:: text

	:name: MCP23S17 Sample
	:description: Custom MCP23S17 Board

	using sysbus

	set PROJECT_BASE $ORIGIN/../../

	i $ORIGIN/visualizer/plugin.py
	i $PROJECT_BASE/renode/drivers/gpio/Mcp23S17.cs

	# Create a machine (and switch to machine context)
	mach create

	# Load platform definition
	machine LoadPlatformDescription $ORIGIN/testbench.repl

	showAnalyzer sysbus.usart2

	macro reset
	"""
			sysbus LoadELF $bin
	"""

	runMacro $reset

Running your simulation
-----------------------

Depending on whether you want to run app specific simulation, basic board
simulation or a testbench, you can do so using designated make targets.

.. code-block:: console

	# run resc for current app under app/boards/<board>.resc
	west build -t appbench
	# run resc for current board under <board>/<board>.resc
	west build -t boardbench
	# run testbench defined under testbenches and enabled in prj.conf
	west build -t testbench

By default, simulation is started directly in the terminal and there is no extra
uart window. You can connect to any uart on your simulated board using renode
command 'uart_connect':

.. code-block:: console

	(machine-0) uart_connect sysbus.usart2

If you want to run the simulation in a more graphical mode then you can use the
same commands with "_xwt" at the end:

.. code-block:: console

	# run resc for current app under app/boards/<board>.resc
	west build -t appbench_xwt
	# run resc for current board under <board>/<board>.resc
	west build -t boardbench_xwt
	# run testbench defined under testbenches and enabled in prj.conf
	west build -t testbench_xwt

The same applies to debugserver. There are dedicated commands for each
simulation to start debugserver automatically (of course you can start it
manually through the monitor as well):

.. code-block:: console

	# run resc for current app under app/boards/<board>.resc
	west build -t appbench_debugserver
	# run resc for current board under <board>/<board>.resc
	west build -t boardbench_debugserver
	# run testbench defined under testbenches and enabled in prj.conf
	west build -t testbench_debugserver

Automating your simulation
--------------------------

Automation is achieved by starting the simulation from a robot framework script.
This functionality is provided by the 'robotbench' target.

Inside your testbench you can see a file called 'testbench.robot':

.. code-block:: text

	*** Settings ***
	Resource                      ${RENODEKEYWORDS}
	Suite Setup                   Setup
	Suite Teardown                Teardown
	Test Setup                    Reset Emulation
	Test Teardown                 Test Teardown

	*** Variables ***
	${PROJECT_BASE}               %{PROJECT_BASE}
	${APPLICATION_BINARY_DIR}     %{APPLICATION_BINARY_DIR}
	${APPLICATION_SOURCE_DIR}     %{APPLICATION_SOURCE_DIR}
	${BOARD}                      %{BOARD}
	${UART}                       sysbus.usart2

	*** Test Cases ***

	Leds can be controlled by buttons
		Set Test Variable  ${GPIO}  GPIO_E0
		Boot
		FOR  ${PIN}  IN RANGE  8  15
			${btn}=  Evaluate  ${PIN} - 8
			Write Line To Uart  gpio get ${GPIO} ${PIN}
			Wait For Line On Uart  Value 0  timeout=2

			Execute Command  sysbus.spi1.gpioExpander.A${btn} Press
			Write Line To Uart  gpio get ${GPIO} ${PIN}
			Wait For Line On Uart  Value 1  timeout=2

			Execute Command  sysbus.spi1.gpioExpander.A${btn} Release
			Write Line To Uart  gpio get ${GPIO} ${PIN}
			Wait For Line On Uart  Value 0  timeout=2
		END

	*** Keywords ***

	Boot
		Execute Command           set bin @${APPLICATION_BINARY_DIR}/zephyr/zephyr.elf
		Execute Command           set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}
		Execute Command           include @${CURDIR}/testbench.resc
		Create Terminal Tester    ${UART}
		Start Emulation
		Wait For Line On Uart		MCP23S17 initialized

This file automates loading of the firmare into a testbench and then executing
actual tests against it. RobotFramework is written in python so it is possible
to include inline python code inside the tests and extend the test using python.

This provides true testing capabilities which resc files on their own lack.
Using robotbench you can run your simulation and tests in CI quite easily.
