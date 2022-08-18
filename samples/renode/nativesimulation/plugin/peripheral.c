// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

#include <stdio.h>
#include <syslog.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <renode/protocol.h>
#include <renode_imports.h>

#define EXPORT __attribute__((visibility("default")))

extern void protocol_respond(void *ptr);
extern void protocol_irq_request(void *ptr);
extern void protocol_wait_request(void *ptr);

EXTERNAL_AS(action_intptr, HandleResponse, protocol_respond);
EXTERNAL_AS(action_intptr, HandleIRQMessage, protocol_irq_request);
EXTERNAL_AS(action_intptr, Receive, protocol_wait_request);

enum register_id {
	REG_IN = 1,
	REG_OUT = 2,
	REG_COUNT = 3,
};

struct peripheral {
	uint32_t regs[REG_COUNT];
};

static struct peripheral inst;

void peripheral_init(struct peripheral *self)
{
	memset(self, 0, sizeof(*self));
}

void peripheral_reset(struct peripheral *self)
{
	printf("Reset\n");
}

void peripheral_transfer(struct peripheral *self, const struct renode_packet *req,
			 struct renode_packet *res)
{
	switch (req->type) {
	case MSG_TYPE_WRITE: {
		if (req->addr == REG_IN) {
			self->regs[REG_OUT] = req->value;
			res->type = MSG_TYPE_OK;
		}
		if (req->addr >= 0 && req->addr < REG_COUNT) {
			self->regs[req->addr] = req->value;
			res->type = MSG_TYPE_OK;
		}
	} break;
	case MSG_TYPE_READ: {
		if (req->addr >= 0 && req->addr < REG_COUNT) {
			res->value = self->regs[req->addr];
			res->type = MSG_TYPE_OK;
		}
	} break;
	}
}

EXPORT void initialize_native(void)
{
	peripheral_init(&inst);
}

EXPORT void handle_request(const struct renode_packet *req)
{
	struct renode_packet res;

	memset(&res, 0, sizeof(res));
	res.type = MSG_TYPE_ERROR;
	peripheral_transfer(&inst, req, &res);
	protocol_respond(&res);
}

EXPORT void reset_peripheral(void)
{
	peripheral_reset(&inst);
}
