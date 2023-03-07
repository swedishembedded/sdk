// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <stdint.h>
#include "serial_ops.h"

/**
 * Generic serial device write
 * \param handle the handle to a serial device
 * \param data data to be written
 * \param size size of data to be written
 * \returns number of bytes written
 **/
int serial_write(const struct serial_ops **handle, const char *data, size_t size)
{
	// here we DO have access to the api so we can use it directly
	return (*handle)->write(handle, data, size);
}
