/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

#include "serial_ops.h"

struct serial_impl {
	const struct serial_ops *serial;
};

void serial_impl_init(struct serial_impl *self);
const struct serial_ops **serial_impl_to_serial(struct serial_impl *self);
