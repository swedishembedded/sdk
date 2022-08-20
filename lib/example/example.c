// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <string.h>
#include <kernel.h>
#include <example/example.h>

int example_object_init(struct example_object *self)
{
	if (!self)
		return -EINVAL;
	memset(self, 0, sizeof(*self));
	printk("Example object initialized!\n");

	k_mutex_init(&self->mx);
	return 0;
}
