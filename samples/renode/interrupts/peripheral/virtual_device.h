/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

#include <stdint.h>

/** This defines our virtual device */
struct virtual_device {
	float u;
	float y;
	float r;
	uint32_t INTF;
} __attribute__((packed)) __attribute__((aligned(4)));
