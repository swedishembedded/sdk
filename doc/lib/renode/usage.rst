.. SPDX-License-Identifier: Apache-2.0
.. Copyright 2022 Martin Schröder <info@swedishembedded.com>
   Consulting: https://swedishembedded.com/go
   Training: https://swedishembedded.com/tag/training

Using this library
##################

So your simulated peripheral executable will be executed like this:

.. code-block:: shell

	peripheral.elf <mainPort> <irqPort> <address>

Inside this executable, you then need to setup this communication using the
functionality provided by this library. This is done as follows:

.. code-block:: c
	int mainPort = atoi(argv[1]);
	int irqPort = atoi(argv[2]);
	const char *address = argv[3];
	struct renode *renode = renode_new();

	if(renode_connect(renode, mainPort, irqPort, address) != 0){
		fprintf(stderr, "Could not connect to renode (IP: %s)\n", address);
		return -1;
	}

Once connected, the communication is always done in request/response manner. The
reason we use two sockets is because we want to enable request/response in both
directions.

The main communication channel is controlled by renode plugin and the plugin
will send requests to our process whenever firmware tries to access memory into
which our peripheral has been mapped. We can handle these requests in a loop
where we do the following:

.. code-block: c

	// wait for a packet (blocking)
	if(renode_wait_request(renode, &req) != 0){
		fprintf(stderr, "Failed to receive packet\n");
		break;
	}

	// ... process packet

	// ... send response
	if(renode_send_response(renode, &res) != 0){
		fprintf(stderr, "Failed to send packet");
		break;
	}

The second socket allows sending out request back to the renode process and
getting a response before sending back the first response. Using two sockets
makes this very easy to accomplish!
