// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <string.h>
#include "serial_ops.h"
#include "serial_impl.h"

static int serial_impl_write(const struct serial_ops **handle, const char *data, size_t size)
{
	// simple implementation disregarding size
	printk("%s", data);
	return size;
}

static const struct serial_ops _ops = { .write = serial_impl_write };

void serial_impl_init(struct serial_impl *self)
{
	memset(self, 0, sizeof(*self));
	self->serial = &_ops;
}

const struct serial_ops **serial_impl_to_serial(struct serial_impl *self)
{
	return &self->serial;
}
