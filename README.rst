**************************************
Swedish Embedded Firmware Platform SDK
**************************************

This is the official Swedish Embedded Platform SDK for building embedded
firmware. It includes infrastructure for comprehensive testing and verification
making development process more robust and predictable.

Applications built using this SDK
#################################

This section will have to be updated once public applications have been added.

Who this SDK is for
###################

This SDK is for you if you have hit the limit of workability with plain bare
metal development or in working with systems like FreeRTOS. This SDK helps you
get to the next level and scale your firmware development by providing a
complete system of infrastructure, reusable components, drivers and
applications that make your development work move along much faster.

This SDK helps you skip the steps you would otherwise have to implement yourself
in setting up a proper infrastructure for development.

Concept behind this SDK
#######################

This SDK extends Zephyr RTOS kernel and adds additional software support on top
of that which makes firmware application development easy. While Zephyr RTOS
kernel is focused on providing a comprehensive, top quality realtime kernel,
this SDK is focused on providing a comprehensive, top quality application
development system for building embedded firmware applications.

What is included
################

This SDK gives you the following:

* **Embedded DevOps Infrastructure**: as defined at
  https://swedishembedded.com/devops/
* **Documentation Infrastructure**: that produces a single PDF and also an HTML
  website with all documentation for your project.
* **Zephyr RTOS Kernel**: with support for over 350 boards, multiple
  architectures and a high quality system architecture that you can build on.
* **System Testing And Simulation**: so you can test your code in a simulated
  environment on Renode as part of your continuous delivery pipeline.
* **Application Development**: complete application development framework so you
  can focus on your business logic.
* **RobotFramework Integration**: this SDK makes heavy use of RobotFramework
  "plain english" test description toolkit to enable comprehensive tests to be
  written for every aspect of the system. RobotFramework tests run as part of
  the CI process.

Who benefits the most from this SDK
###################################

- **Embedded Firmware Consultants**: if you are an embedded firmware consultant
  then this SDK helps you compete on the marketplace by being able to offer more
  to your customers when developing new firmware and migrating old code to a
  realtime RTOS.
- **Hardware Developers**: this SDK helps you quickly add hardware support for
  your new hardware so you can provide your customers with a powerful platform
  to build their applications on.

Benefits of starting with this SDK
##################################

Choosing this SDK for your continued firmware development also gives you the
following benefits:

* **Training**: all courses at https://swedishembedded.com/training are
  available to accelerate the development process. Using another SDK would
  create difficulties for you.
* **Comprehensive Hardware Support**: you can leverage open source technology to
  get support for existing hardware which can be adopted to your project.
* **100% Tested**: this SDK requires that every single line of source code
  within it is 100% unit tested. This is a hard requirement enforced by CI
  process. Giving you a highly predictable and stable system.
* **Emulation**: simulation is part of the development process with this SDK.
  You can simulate your firmware on native posix or on virtual hardware as
  supported by Renode.

How is this SDK different
#########################

The are many other SDKs out there for building embedded firmware. This SDK picks
the best ideas and concepts and combines the best of open source firmware
ecosystem which results in simply a better SDK.

How to get started
##################

This section will be expanded once more examples have been added.
