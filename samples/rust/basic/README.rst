.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

.. _rust_basic:

Rust Basic Sample
#################

Overview
********

This sample shows minimal code required to include rust code as a library into
your Zephyr application. The main method of this sample calls a rust function,
then the rust function calls Zephyr printk function to print a message and then
calls a C function in the main application.

Building and Running
********************

This application can be built and executed on QEMU as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/rust/basic
   :host-os: unix
   :board: qemu_x86
   :goals: run
   :compact:

To build for another board, change "qemu_x86" above to that board's name.

Sample Output
=============

.. code-block:: console

    Message from RUST: Hello World! 123
    Message from C: Hello World! qemu_x86_64

Exit QEMU by pressing :kbd:`CTRL+A` :kbd:`x`.
