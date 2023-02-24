// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include "button.h"

#include <zephyr/sys/slist.h>
#include <string.h>

void button_init(struct button *self)
{
	memset(self, 0, sizeof(*self));
}

void button_deinit(struct button *self)
{
	self->cb = NULL;
}

size_t button_size(void)
{
	return sizeof(struct button);
}

void button_set_callback(struct button *self, struct button_callback *cb)
{
	self->cb = cb;
}

void button_remove_callback(struct button *self, struct button_callback *cb)
{
	self->cb = NULL;
}

void button_add_callback(struct button *self, struct button_callback *cb)
{
	sys_slist_append(&self->callbacks, &cb->node);
}

void button_delete_callback(struct button *self, struct button_callback *cb)
{
	sys_slist_find_and_remove(&self->callbacks, &cb->node);
}

void button_do_something(struct button *self)
{
	// call the callback
	if (self->cb)
		self->cb->cb(self->cb);

	sys_snode_t *node;

	SYS_SLIST_FOR_EACH_NODE (&self->callbacks, node) {
		// once again we use container of here since we know node points to cb->node
		struct button_callback *cb = CONTAINER_OF(node, struct button_callback, node);

		cb->cb(cb); // call the callback
	}
}
