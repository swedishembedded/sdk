.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Development workflow
********************

This section describes in detail the process of developing applications and
verifying them using the tools provided by this SDK.

Having a well defined workflow helps you stay organized and ensures that you do
not forget important steps of the process.

In this section we are going to cover the following important steps:

- **Setting up a new project**: here we will setup a simple embedded application
  and run it inside the simulation. Very similar to what you have seen in the
  introduction - but here the details of the process are explained.
- **Building your application for target platforms**: this section explains what
  you need to do in order to make your application support multiple platforms
  (such as multiple boards or multiple products).
- **Debugging your application**: this section deals with the details of
  debugging. Here I show you how to get various debugging information from your
  running program and the tools that the SDK provides for this purpose.
- **Simulating your application**: here we look at how the simulation framework
  is setup and how you can add new simulation environments to your application.

After becoming familiar with the workflow you should be able to use this SDK to
build, simulate and test your embedded application with ease.
