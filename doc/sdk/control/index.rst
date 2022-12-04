.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Control Systems Toolbox
#######################

This SDK includes a complete control systems toolbox written in C and Octave.
This toolbox is included as a module through west and resides in its own
`separate repository`_ (when you initialize the SDK you get access to it
automatically just like all the other modules!).

.. _separate repository: https://github.com/swedishembedded/control

The control systems toolbox gives you a fairly complete set of tools to design
advanced control algorithms (including model based controllers that use kalman
filters) in pure C such that they can be used as part of a fast control path
where every cycle counts.

There is more documentation at the moment on the control systems toolbox github
page, as well as in the doc section of that repository.
