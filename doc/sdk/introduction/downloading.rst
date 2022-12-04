.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Downloading Docker Image And Source Code
========================================

The SDK is provided to you in two ways:

- **Docker images**: there are two images: **build** image and **develop**
  image. The build image contains everything necessary to run builds in CI
  (including simulation testbench and RobotFramework user scenarios). The
  develop image is a simple integrated development environment which can be used
  for terminal based development (if you want to use VSCode or some other
  graphical editor, you can use edit your code locally and use this image for
  testing and debugging).

- **Source code**: the source code of the SDK can be used for working with the
  SDK completely outside of the docker image. This repository contains a
  separate installation script which installs all the necessary tools that are
  also available as part of the docker image. This repository can be used
  standalone, but using the docker image guarantees that there are no errors
  during configuration.

Using the docker image to build and run a sample application
------------------------------------------------------------

First you need to pull the docker image. This image consists of several "layers"
or "parts" (which is how docker works) and contains a development environment.

.. code-block:: shell

	$ docker pull swedishembedded/develop:latest

Once the image is downloaded, you can now start the docker container. A docker
container is like a virtual machine - but the difference is that it runs
natively - so there is no performance hit when working inside the container.

.. code-block:: shell

	$ docker run -ti swedishembedded/develop:latest
	SDK /build>

You are now inside the docker container system. Everything inside this system is
disposable. When you stop and restart your docker container, all your changes
will be reverted. This is intentional.

If you want to prevent this, you should make one of your local directories
available inside docker.

.. code-block:: shell

	$ docker run -ti -v /local/path:/docker/path swedishembedded/develop:latest
	SDK /build> ls -la /docker/path

You will now see your local files under /docker/path. Making any changes here
will directly effect your local files on your computer!

You can now build and run a sample application from inside the docker container:

.. code-block:: shell

	SDK /build> cd platform/sdk
	SDK /build/platform/sdk> west build -p -b custom_board apps/shell

This cross compiles a shell application and produces a binary for STM32.

You can now run this binary in the simulation by simply typing:

.. code-block:: shell

	SDK /build/platform/sdk> west build -t appbench

This will run the application specific testbench. There are several testbench
methods available:

- **robotbench**: this is used for running robot framework scripts in CI for a
  product specific testbench.
- **testbench**: this invokes the product specific testbench configured through
  project Kconfig option.
- **boardbench**: this runs the default minimal board simulation (without
  external peripherals) provided by the board definition.
- **appbench**: this runs an app specific simulation which uses app specific
  platform definition (provided by the application) and contains all peripherals
  that are specified by the app for target board. This is suitable for
  debugging.

Once you have executed appbench you will be in the simulation monitor prompt.
This prompt is very powerful.

You can type "help" to get help:

.. code-block:: shell

	(machine-0) help
	alias             : sets an alias.
	allowPrivates     : allow private fields and properties manipulation.
	analyzers         : shows available analyzers for peripheral.
	commandFromHistory: executes command from history.
	createPlatform    : creates a platform.
	currentTime       : prints out and logs the current emulation virtual and real time
	displayImage      : Displays image in Monitor
	execute           : executes a command or the content of a variable.
	help              : prints this help message or info about specified command.
	history           : prints command history.
	include           : loads a monitor script, python code or a plugin class.
	lastLog           : Logs last n logs.
	log               : logs messages.
	logFile           : sets the output file for logger.
	logLevel          : sets logging level for backends.
	mach              : list and manipulate machines available in the environment.
	macro             : sets a macro.
	numbersMode       : sets the way numbers are displayed.
	path              : allows modification of internal 'PATH' variable.
	pause             : pauses the emulation.
	peripherals       : prints list of registered and named peripherals.
	python            : executes the provided python command.
	quit              : quits the emulator.
	require           : verifies the existence of a variable.
	runMacro          : executes a command or the content of a macro.
	set               : sets a variable.
	showAnalyzer      : opens a peripheral backend analyzer.
	start             : starts the emulation.
	string            : treat given arguments as a single string.
	using             : expose a prefix to avoid typing full object names.
	verboseMode       : controls the verbosity of the Monitor.
	version           : shows version information.

For example, while your application is running you can log function names into a
file:

.. code-block:: shell

	(machine-0) logFile @/build/platform/sdk/foo.log
	(machine-0) logLevel 0 file
	(machine-0) sysbus.cpu LogFunctionNames true

In the log file you will then see every single function call that your firmware
makes:

.. code-block:: shell

	14:42:41 [INFO] cpu: Entering function sys_clock_announce at 0x8004CD0
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout (entry) at 0x8003A2C
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout at 0x8003A4A
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout at 0x8003A50
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout at 0x8003A54
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout at 0x8003AA2
	14:42:41 [INFO] cpu: Entering function sys_clock_set_timeout at 0x8003A5E

Every emulated peripheral has it's own log level. You see a list of peripherals
by using the "peripherals" command. Run "logLevel" without any arguments to see
what log levels are currently defined.

This is only a fraction of what the emulation monitor can do. Use tab completion
inside the monitor window to get hints of what methods you can call. Try it on
sysbus peripherals:

.. code-block:: shell

	(machine-0) sysbus.<tab>

Downloading and using SDK without Docker
----------------------------------------

Even if docker provides a fast way to get started, it is often convenient to
configure the SDK and all the tools on your local system.

First you need to create a workspace directory under which all tools will be
downloaded. You will check out the SDK source code into the empty workspace
directory and then SDK init script will populate the same directory with other
dependencies.

.. code-block::

	mkdir workspace && cd workspace
	git clone https://github.com/swedishembedded/sdk
	cd sdk

You can now use scripts provided by the SDK to initialize your local build
environment without using docker.

To execute the whole build process manually on your computer you can use
following commands:

- **./scripts/install-sdk**: install toolchains and compilation tools
- **./scripts/init**: insitialize the workspace and pull dependencies source code
- **robot scrum**: run scrum tests
- **./scripts/check**: run all source code checks
- **./scripts/build**: build all applications and samples
- **./scripts/test**: build and run all unit and integration tests
- **robot post-build**: execute post build compliance tests on build output

The first command prepares your system for development. The other commands are
important to know about because they are run as part of CI process and you can
run them manually before pushing to ci in order to check if everything builds
properly.

