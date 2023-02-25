// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <string.h>

struct base_one {
	uint32_t x;
};

struct base_two {
	uint32_t x;
};

struct derived {
	struct base_one base_one;
	struct base_two base_two;
};

void base_one_init(struct base_one *self)
{
	memset(self, 0, sizeof(*self));
}

void base_one_do_something(struct base_one *self)
{
	printk("%s\n", __func__);
}

void base_two_init(struct base_two *self)
{
	memset(self, 0, sizeof(*self));
}

void derived_init(struct derived *self)
{
	base_one_init(&self->base_one);
	base_two_init(&self->base_two);
}

void derived_do_something(struct derived *self)
{
	printk("%s\n", __func__);
	base_one_do_something(&self->base_one);
}

struct trait_x {
	void (*do_x)(struct trait_x *self);
};

struct trait_y {
	void (*do_y)(struct trait_y *self);
};

static inline void trait_x_do(struct trait_x *self)
{
	self->do_x(self);
}

static inline void trait_y_do(struct trait_y *self)
{
	self->do_y(self);
}

struct derived_with_traits {
	struct trait_x trait_x;
	struct trait_y trait_y;
};

void derived_with_traits_do_x(struct trait_x *trait)
{
	struct derived_with_traits *self = CONTAINER_OF(trait, struct derived_with_traits, trait_x);

	printk("%p: %s\n", self, __func__);
}

void derived_with_traits_do_y(struct trait_y *trait)
{
	struct derived_with_traits *self = CONTAINER_OF(trait, struct derived_with_traits, trait_y);

	printk("%p: %s\n", self, __func__);
}

void derived_with_traits_init(struct derived_with_traits *self)
{
	self->trait_x = (struct trait_x){ .do_x = derived_with_traits_do_x };
	self->trait_y = (struct trait_y){ .do_y = derived_with_traits_do_y };
}

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
	struct derived derived;

	derived_init(&derived);
	derived_do_something(&derived);

	struct derived_with_traits dwt;

	derived_with_traits_init(&dwt);

	struct trait_x *dwx = &dwt.trait_x;
	struct trait_y *dwy = &dwt.trait_y;

	trait_x_do(dwx);
	trait_y_do(dwy);
}
