.. SPDX-License-Identifier: Apache-2.0
	 Copyright 2022 Martin Schröder

Peripheral Simulation Using Native Shared Library
#################################################

This sample shows how to write a plugin for interfacing with a shared library
allowing you to memory map your new simulated peripheral into your
microcontroller address space in simulation.

.. code-block:: text

	cperipheral: SharedLibraryPeripheral @ sysbus <0x70000000, +0x100>

Which allows us to then access our peripheral from the firmware side directly as
though it was mapped into processor address space:

.. code-block:: c

	#define REG_BASE (volatile uint32_t *)0x70000000;
	#define REG_IN *(REG_BASE + 0x01)
	#define REG_OUT *(REG_BASE + 0x02)

	REG_IN = 555;
	printf("REG_OUT = %08x\n", REG_OUT);

This gives us flexibility to create powerful simulation tools that run along
side of our firmware such as dynamic model simulation, physics simulation or
other types of sensor simulation.
