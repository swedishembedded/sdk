.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Working on Windows
==================

The SDK should build and run fine on windows - although the process is not
tested very often.

To use the SDK and docker image on windows you will need two things:

- **Windows subsystem for Linux v2 (WSL)**: this is an ubuntu setup that runs
  natively on windows. It gives you a fully ubuntu compatible terminal on
  windows. You can get it here: https://ubuntu.com/wsl

- **Windows Docker Desktop**: this provides windows docker support. When you
  install docker desktop you will have access to the docker command from the WSL
  linux terminal. Get it here: https://docs.docker.com/desktop/windows/wsl/

Once installed, the rest of the instructions for setting up the SDK and using it
are exactly the same.
