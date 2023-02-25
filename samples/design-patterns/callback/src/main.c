// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <zephyr/sys/check.h>
#include <zephyr/sys/dlist.h>
#include <assert.h>
#include <string.h>
#include "button.h"

struct controller {
	/** Button instance */
	struct button *btn;
	/** Button callback */
	struct button_callback cb; // our callback
};

static void controller_on_button_cb(struct button_callback *cb)
{
	// this is how we can now get pointer to instance of B
	// notice that the callback is generic
	struct controller *self = CONTAINER_OF(cb, struct controller, cb);

	printk("Controller callback for button %p\n", self->btn);
}

void controller_init(struct controller *self, struct button *a)
{
	self->btn = a;
	self->cb.cb = controller_on_button_cb;
	/* Alternative 1 */
	button_set_callback(a, &self->cb);
	/* Alternative 2 */
	button_add_callback(a, &self->cb);
}

void controller_deinit(struct controller *self)
{
	/* Alternative 1 */
	button_remove_callback(self->btn, &self->cb);
	/* Alternative 2 */
	button_delete_callback(self->btn, &self->cb);
}

void main(void)
{
	struct controller ctrl;
	struct button btn;

	button_init(&btn);
	controller_init(&ctrl, &btn);

	// this will call controller callback function
	button_do_something(&btn);

	controller_deinit(&ctrl);
	button_deinit(&btn);
}
