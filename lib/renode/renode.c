// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/consulting
 * Training: https://swedishembedded.com/tag/training
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifdef __linux__
#include <unistd.h>
#else
#include <net/socket.h>
#endif

#include <renode/renode.h>

struct renode {
	/** Main socket from simulator to us (req/res) */
	int mainSocket;
	/** Special socket from us to simulator (req/res) */
	int irqSocket;
};

struct renode *renode_new(void)
{
	struct renode *self = (struct renode *)malloc(sizeof(struct renode));

	memset(self, 0, sizeof(*self));
	return self;
}

void renode_free(struct renode **self)
{
	free(*self);
	*self = NULL;
}

int renode_connect(struct renode *self, int mainPort, int irqPort, const char *address)
{
	self->mainSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (self->mainSocket == -1) {
		return -ECONNREFUSED;
	}

	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, address, &addr.sin_addr);
	addr.sin_port = htons(mainPort);

	if (connect(self->mainSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		close(self->mainSocket);
		return -ECONNREFUSED;
	}

	self->irqSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (self->irqSocket == -1) {
		return -ECONNREFUSED;
	}

	addr.sin_port = htons(irqPort);

	if (connect(self->irqSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		fprintf(stderr, "Could not connect to %s:%d\n", address, irqPort);
		close(self->mainSocket);
		close(self->irqSocket);
		return -ECONNREFUSED;
	}
	return 0;
}

int renode_disconnect(struct renode *self)
{
	close(self->mainSocket);
	close(self->irqSocket);
	return 0;
}

int renode_wait_request(struct renode *self, struct renode_packet *req)
{
	int r = recv(self->mainSocket, req, sizeof(*req), 0);

	if (r < 0) {
		return -EIO;
	} else if (r != sizeof(*req)) {
		return -EINVAL;
	}
	return 0;
}

int renode_send_response(struct renode *self, struct renode_packet *res)
{
	int r = send(self->mainSocket, res, sizeof(*res), 0);

	if (r < 0) {
		return -EIO;
	} else if (r != sizeof(*res)) {
		return -EINVAL;
	}
	return 0;
}

int renode_irq_notify(struct renode *self)
{
	struct renode_packet res;

	res.type = MSG_TYPE_IRQ;

	int r = send(self->irqSocket, &res, sizeof(res), 0);

	if (r < 0) {
		return -EIO;
	} else if (r != sizeof(res)) {
		return -EINVAL;
	}
	return 0;
}
