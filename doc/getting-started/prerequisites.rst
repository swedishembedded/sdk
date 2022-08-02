Required software
=================

You should have docker installed locally and you should be able to download and
run docker containers using the docker command.

If you are on windows, you will have to install windows subsystem for linux
(WSL) version 2 and docker desktop for windows. This will provide you with the
docker command.

On ubuntu you can install docker from apt:

.. code-block::

	sudo apt install docker

After installing docker you should be able to pull the Swedish Embedded
Development SDK image:

.. code-block::

	docker pull swedishembedded/develop:latest

It will take a while to download the image and once it is all downloaded and
extracted, you can proceed to the next step.

The docker image includes everything you need for development. This includes
compilers, build tools, and even a copy of the SDK repository under
/build/platform/sdk (in the docker image filesystem).

A sidenote: the source code contained in the development docker image is for
demo purposes only. When you work with the tools inside the docker image you
should be working on source code that is stored outside of the docker image. You
can later accomplish this by "mounting" a local directory inside the docker
filesystem using the docker "-v" option.
