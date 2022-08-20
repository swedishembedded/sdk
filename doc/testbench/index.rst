.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Testbench
#########

Testbenches are simulation environments which can be used to run your firmawre.
These are like physical boards, but implemented entirely in software using
powerful simulation provided by Renode simulator. Testbenches are like mini-labs
with a fully connected fixture defined using scripts.

Testbenches provide you with:

- **Replecation of your physical hardware**: so you don't need to use physical
  hardware for development.

- **Automation of the testing process using RobotFramework**: so that you can
  execute tests in CI easily.

Testbenches are defined under "testbench" directory.

- **Fixed board configuration**: Each testbench is a specific fixed board
  configuration with specific features. A testbench works like a virtual
  deployment environment. It is typically designed to run a specific kind of
  firmware - although may be used for several firmwares.

- **User perspective**: Testbench tests functionality from the perspective of
  the user. Whatever user is able to do with your application should also be
  expressable in the form of testbench automation.

- **Chosen at build time**: Testbench is chosen through Kconfig (or prj.conf)
  when you configure your build. This defines extra build targets that can be
  used to run the tests (testbench and robotbench).

- **Defines scope of functionality**: Testbench defines the scope of what
  features must be supported by the application. If application fails to
  implement some features required by a testbench then the testbench run fails.

- **Interactive and automatic**: Testbench can be executed either in interactive
  or automatic mode.

- **Implements system testing**: Testbench runs production level firmware and is
  thus considered to be a form of "black box", end-to-end testing of your whole
  product - but done in the form of simulation.

When a testbench runs, it is designed to be used to test every aspect of a
particular product related to the deployment environment that it represents. An
application is considered "compliant" when it implements all the functionality
required by the testbench that is used for verifying it.

Configuring your application to use a testbench:

.. code-block::

	 west build -p -b custom_board -s <path> --
	 -DCONFIG_TESTBENCH_NUCLEO401RE_MCP23S17=y
	 west build -t testbench # run interactive
	 west build -t robotbench # run automatic

The test bench tests application features as they are viewed by the user. Your
application should already have passed two main test stages:

- Unit tests should have verified the logical functionality of each function.

- Integration tests should have verified functional compliance when your code is
  interfaced with other modules.

Testbench implements system testing. While there can be multiple testbenches
running the same firmware, each build can be configured to use only one test
bench.

This design is intentional because one product should map to one test bench.
When you add features to your product, you should only have to worry about
extending a single test bench and not have to deal with system level tests
scattered around many places.

When compliance tests run on the testbench for one particular release of the
software, a RobotFramework report is generated. This report can be included with
the release as a way of showing what functionality has been tested in the case
of that particular build.

.. toctree::
   :maxdepth: 1
   :caption: Contents

   nucleo401re_mcp23s17/doc/index.rst

Running testbench in Twister
****************************

Testbenches can be run in twister. This requires that you have defined
simulation type for your board and set it to "robotbench" in your board yml:

.. code-block::

	 simulation: robotbench

You can then run this kind of simulation by simply executing twister:

.. code-block::

	 ../zephyr/scripts/twister \
		-p custom_board \
		-c -O build-apps -v -i -A boards \
		-T samples/drivers/gpio/mcp23s17/

This presupposes that custom_board has been configured to use robotbench as
simulation and that your project has been configured to use a testbench.
