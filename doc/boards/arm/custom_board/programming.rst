Programming and Debugging
*************************

Applications for the ``nucleo_f401re`` board configuration can be built and
flashed in the usual way.

Flashing
========

Nucleo F401RE board includes an ST-LINK/V2-1 embedded debug tool interface.
This interface is supported by the openocd version included in Zephyr SDK.

Flashing an application to Nucleo F401RE
----------------------------------------

Connect the Nucleo F401RE to your host computer using the USB port,
then run a serial host program to connect with your Nucleo board:

.. code-block:: console

   $ minicom -D /dev/ttyACM0

Now build and flash an application.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: nucleo_f401re
   :goals: build flash

You should see the following message on the console:

.. code-block:: console

   Hello World! arm


Debugging
=========

You can debug an application in the usual way.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: nucleo_f401re
   :maybe-skip-config:
   :goals: debug
