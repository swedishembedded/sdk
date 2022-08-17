.. SPDX-License-Identifier: Apache-2.0
	 Copyright 2022 Martin Schröder

LQI Controller With Standalone Simulation
#########################################

This sample shows implementation of an LQI controller using the Swedish Embedded
Control Systems Toolbox (which is included in the SDK).

The controller uses a predefined model which is used to execute lqi algorithm
for control and kalman filter for state estimation.

The plant runs in a separate process which is mapped and accessible to the
simulation over a socket interface.

.. code-block:: text

	model: SocketPeripheral @ sysbus <0x70000000, +0x100>

This maps the model of the plant as a peripheral into the firmware address
space, allowing us to use simple read/write operations to memory in order to
interact with the plant.

.. code-block:: c

	//Control LQI
	lqi(y, u, qi, r, L, Li, x, xi, ADIM, YDIM, RDIM, ANTI_WINDUP);

	// Estimate the state x
	kalman(A, B, C, K, u, x, y, ADIM, YDIM, RDIM);

	// Write control input to the plant
	PLANT_U0 = (uint32_t)(u[0] * 0xFFFF);
	// Read measurement from the plant
	y[0] = ((float)PLANT_Y0 / 0xFFFF);

You can run this simulation using the 'appbench' command which will start the
simulation and redirect uart to the console:

.. code-block:: shell

	west build -p -b custom_board samples/lib/control/basic
	west build -t appbench
