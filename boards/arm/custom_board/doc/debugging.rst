Debugging
*********

Debugging can be done both using the physical board and inside a simulation.

To debug the physical board use:

.. zephyr-app-commands::
   :zephyr-app: apps/shell
   :board: nucleo_f401re
   :maybe-skip-config:
   :goals: debug

To debug inside a simulation use:

.. code-block:: console

  west -t boardbench_debugserver

Then connect to the debugserver using GDB.
