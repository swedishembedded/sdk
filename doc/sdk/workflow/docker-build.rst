.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Building your code using docker
===============================

Docker is by far the preferable way to build. You can create a bash function
that you put into your .bashrc that looks like this:

.. code-block:: bash

	function drun() {
		if [[ ! -d $PWD/../.west/ ]]; then
				echo "Run this command from a directory inside a west workspace!"
				exit 1
		fi
		docker run -ti -v /dev/bus/usb:/dev/bus/usb \
			-v $(realpath $PWD/..):$(realpath $PWD/..) \
			swedishembedded/develop:latest \
			bash -c "cd $PWD && $*"
	}

If you don't want to logout and then login again you can just source your bashrc
and then you can use this function:

.. code-block:: bash

	. ~/.bashrc
	drun west build -p -b custom_board apps/shell

This will build your local source code inside docker. This type of docker usage
is extremely powerful because you can setup a full environment inside docker and
then use the docker image as your primary way of building code. In that
environment you can specific versions of different tools that are compatible
with your project. It becomes, in effect, a fully reproducible process.

.. code-block:: bash

	drun make -C doc pdf

This builds the documentation.

Bash functions and aliases are extremely useful!
