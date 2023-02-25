/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/sys/slist.h>
#include <stddef.h>

struct button {
	/** Alternative 1: single callback */
	struct button_callback *cb;
	/** Alternative 2: multiple callbacks */
	sys_slist_t callbacks;
};

// this is a public callback definition
struct button_callback {
	sys_snode_t node;
	void (*cb)(struct button_callback *cb);
};

void button_init(struct button *self);
void button_deinit(struct button *self);
size_t button_size(void);
void button_set_callback(struct button *self, struct button_callback *cb);
void button_remove_callback(struct button *self, struct button_callback *cb);
void button_add_callback(struct button *self, struct button_callback *cb);
void button_delete_callback(struct button *self, struct button_callback *cb);
void button_do_something(struct button *self);
