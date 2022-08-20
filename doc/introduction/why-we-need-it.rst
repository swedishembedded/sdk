.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Unites several professional tools and frameworks
================================================

The highly modular design of this SDK means that many existing libraries can be
added as extensions and cross compiled for the existing platforms. This SDK
already integrates several major components that you can readily use in your
application development:

- **Zephyr RTOS Kernel**: this provides advanced realtime kernel support which
  provides scheduling, system primitives and a powerful abstraction layer for
  developing new device drivers and application components for your embedded
  application. Being fully embraced and supported by Google,
  NordicSemiconductor and the Linux Foundation, Zephyr Kernel makes the most
  advanced kernel functionality easily available to your to build on. This SDK
  imports Zephyr as a module and ensures that you can build and link your
  application with Zephyr.
- **Renode Advanced Embedded Simulator**: having been developed for over 10
  years and used for advanced FPGA design and testing, the Renode simulator
  makes it easy to extend your embedded project with custom virtual devices that
  work together with your application code and then allow easy monitoring of the
  internal state of the devices. We use Renode in this SDK because it provides a
  solid foundation for building more advanced simulation and testing tools.
  Renode is based on QEmu core emulation library but adds a tremendous level of
  ease of use on top through C# and Python scripting.
- **CMock and Unity**: CMock is a unique unit testing framework for C which
  takes a completely different approach to what has usually been done. CMock
  allows us to test and verify things at the lowest level - which means ANY and
  ALL code can be successfully verified without running into issues associated
  with difficulty to reach code that only runs when certain hardware conditions
  occur. CMock integration ensures that we can do 100% test coverage for your
  embedded applications (At the time of this writing I have created a merge
  request to merge CMock support into Zephyr but it has not been merged yet)

Why do we need an extra SDK? Why not just use these tools separately or
contribute to them?

While the answer to this question is simple, let's look at what firmware
development workflow typically requires:

- **CI pipeline**: that integrates all tools with project specific code, builds
  the firmware, runs all tests and spits out a pass or fail. This must be
  integrated with GitLab or GitHub such that it can be automatically executed
  before every merge request for the project is merged.
- **Development that targets products**: we need an ability to build testbenches
  for physical products, which may contain multiple technologies. Thus the tools
  above are shared and must be integrated in one place in such a way that CI can
  be used to verify product compliance. The tools above target architectures and
  chips - while at a higher level we must target products and complete networks
  of devices that compose a "product" or "service" (for example, parking meters
  are a "parking service". When you want to simulate a "parking service" you
  must simulate many different meters and devices interacting with eachother).

This SDK addresses these needs by integrating the tools together so that it
becomes possible to build networks of products and verify them fully.
