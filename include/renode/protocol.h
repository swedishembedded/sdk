/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/consulting
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

#include <stdint.h>

struct renode_packet {
	/** Packet type (renode_message_type) */
	uint32_t type;
	/** Address for current operation (if applicable) */
	uint64_t addr;
	/** Value for current operation (if applicable) */
	uint64_t value;
} __attribute__((packed, aligned(1)));

enum renode_message_type {
	MSG_TYPE_INVALID = 0,
	MSG_TYPE_TICK_CLOCK = 1,
	MSG_TYPE_WRITE = 2,
	MSG_TYPE_READ = 3,
	MSG_TYPE_RESET = 4,
	MSG_TYPE_IRQ = 5,
	MSG_TYPE_ERROR = 6,
	MSG_TYPE_OK = 7,
	MSG_TYPE_DISCONNECT = 8,
	MSG_TYPE_HANDSHAKE = 9,
};
