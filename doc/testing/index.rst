.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

.. _testing_index:

Test Infrastructure Documentation
#################################

Swedish Embedded SDK and packages that depend on it rely on CMock and Unity for
low level unit testing of code logic.

This approach involves additional scripts and tools which are implemented in
the `Testing`_ repository.

This system predominantly works by generating mocks from existing zephyr files.
You can check the `SDK tests`_ to get an idea how each test application is put
together.

The topics below are imported directly from CMock and Unity documentation:

.. _Testing: https://github.com/swedishembedded/testing
.. _SDK tests: https://github.com/swedishembedded/sdk/tree/main/tests

.. toctree::
   :maxdepth: 1
   :caption: Contents

   CMock_Summary.md
   UnityGettingStartedGuide.md
   UnityConfigurationGuide.md
   UnityAssertionsReference.md
   UnityHelperScriptsGuide.md
