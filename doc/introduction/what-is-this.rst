.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Development, testing and simulation toolbox
===========================================

Swedish Embedded Platform SDK is a development, testing and simulation toolbox
for developing high performance embedded applications. The source code you find
inside the main repository is only a fraction of what it pulls in and makes
easily available to you when you are building your embedded application.

The main SDK image contains 15GB of tools and source code. This includes
toolchain for several modern architectures:

- **aarch64**: which can be used for cross compiling 64 bit ARM binaries.
- **arc64**: cross compile for 64 bit Argonaut RISC Core processors.
- **arc**: cross compile for 32 bit Argonaut RISC Code
- **arm**: cross compile for ARM (STM32, NXP etc)
- **mips**: cross compile for MIPS processor.
- **nios2**: cross compile for the NIOS2 FPGA soft-core
- **riscv64**: cross compile for RISCV 64 bit cores.
- **sparc**: cross compile for SPARC architectures.
- **x86_64**: natively compile your software for debugging and testing
- **xtensa**: cross compile for XTensa

Also included and easily available to you:

- **700+ device drivers**: device drivers for everything from STM32 ADC to touch
  panels and more complex devices.
- **420+ boards**: extensive support for building applications on existing
  boards - and you can easily add your own!
- **40+ simulation platforms**: predefined simulation platforms for simulating
  your production firmware. More support added regularly.
- **HAL support**: extensive HAL support for each of the above architectures.
- **Library support**: including tensorflow, LoRAMAC, CANOpen, GUI etc. New
  libraries can be easily added and cross compiled for any of the supported
  architectures.

What is being actively developed:

- **Testbench framework for building custom testbenches**: these extend the
  existing architecture and simulation support with custom visualization and
  debugging tools - such as mathematically correct dynamic system simulation for
  applications that implement control algorithms. We can build these because we
  have such a powerful simulation infrastructure already in place!
- **Extensive testing infrastructure support**: SDK introduces advanced testing
  and mocking support on multiple levels allowing you to test every aspect of
  your application - both by use of low level mocking and through simulation.
  The SDK itself applies 100% code and branch coverage for all functional code
  included in the SDK. This used to be traditionally hard to achieve for
  embedded frameworks, but this difficulty is no longer the case with the new
  infrastructure.

Testbenches can consist of multiple nodes - meaning that a test bench can be
defined as a network of connected devices where each device runs a different
firmware. A testbench defined this way corresponds to a single testing
environment where each of these devices can be controlled, monitored at CPU
register level and modified at runtime. This provides for a very powerful
framework for realistic simulation of what is happening in the real deployment
environment.

What is on the roadmap:

- **Control systems algorithms support**: given the simulation and testing
  capabilities of the SDK that are in place already, we can add generic building
  blocks for developing advanced control for industrial systems (motors,
  manufacturing machines, car engines, avionics etc). This needs to be added as
  necessity arises.
