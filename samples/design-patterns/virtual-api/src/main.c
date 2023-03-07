// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <string.h>
#include <stdint.h>

#include "serial.h"
#include "serial_impl.h"

/**
 * \brief Output one character
 * \param ch character to print
 **/
void serial_out(uint8_t ch)
{
	printk("%c", ch);
}

size_t rust_main(void);

void main(void)
{
	rust_main();
	printk("--- C version ---\n");

	// initialization of implementation (usually would be done separately from usage below)
	struct serial_impl serial;

	serial_impl_init(&serial);

	// get the handle that can be passed around and used with the generic api
	const struct serial_ops **handle = serial_impl_to_serial(&serial);

	const char *data = "Hello World!\n";

	serial_write(handle, data, strlen(data));
}
