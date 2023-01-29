// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>

void print_message_from_rust(void);

void print_message_from_c(void)
{
	printk("Message from C: Hello World! %s\n", CONFIG_BOARD);
}

void main(void)
{
	print_message_from_rust();
}
