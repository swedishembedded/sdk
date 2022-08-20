.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Programming
***********

You can run applications on this board both physically and through simulation.

Flashing
========

To flash the example shell application to this board, run:

.. code-block:: bash

  west build -p -b custom_board apps/shell -t flash

You can connect to the Nucleo F401RE to your host computer using the USB port:

.. code-block:: console

   $ picocom /dev/ttyACM0

Simulation
==========

In the Swedish Embedded SDK this board supports simulation:

.. code-block:: console

  west build -t boardbench

This runs the default board testbench.

To connect to the onboard uart use this in the simulation monitor:

.. code-block:: console

  (machine-0) uart_connect sysbus.usart2
