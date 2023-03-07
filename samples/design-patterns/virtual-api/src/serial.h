/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

#include <stddef.h>

struct serial_ops;

int serial_write(const struct serial_ops **handle, const char *data, size_t size);
