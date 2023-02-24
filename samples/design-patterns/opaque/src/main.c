// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <zephyr/sys/check.h>
#include <assert.h>
#include <string.h>
#include "opaque.h"

struct opaque *do_stack_allocation(void)
{
	struct opaque *obj = alloca(opaque_size());

	opaque_init(obj);
	opaque_set_data(obj, 123);
	printk("Stack allocated: obj = %p, data = %d\n", obj, opaque_get_data(obj));
	opaque_deinit(obj);
	// at this point the opaque is discarded because the function returns
	// returning this variable is not valid!
	// This will generate compiler error:
	//return obj;
	return NULL;
}

struct opaque *do_heap_allocation(void)
{
	struct opaque *obj = opaque_new();

	__ASSERT(obj, "Memory allocation failed!");
	opaque_init(obj);
	opaque_set_data(obj, 456);
	printk("Heap allocated: obj = %p, data = %d\n", obj, opaque_get_data(obj));
	// returning this object is fine but caller must delete it!
	return obj;
}

void main(void)
{
	do_stack_allocation();
	struct opaque *heap = do_heap_allocation();

	// now we can see what value this prints (remember it was stack allocated!)
	printk("Heap pointer data (should be 456): %d\n", opaque_get_data(heap));

	opaque_deinit(heap);
	// we can free the pointer
	opaque_free(&heap);
}
