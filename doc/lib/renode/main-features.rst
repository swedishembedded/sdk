.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Simulation Interface
####################

When you define a simulation platform in a repl file, it is also possible to
define SocketPeripheral device:

.. code-block:: text

	model: SocketPeripheral @ sysbus <0x70000000, +0x100>

In your resc file you can then set the path to the executable which will
actually be invoked as "handler" for this memory region.

.. code-block:: text

	model PeripheralPath $ORIGIN/../peripheral/peripheral.elf

It is this executable that requires this library in order to setup a connection
back to the emulator and establish two way communication.
