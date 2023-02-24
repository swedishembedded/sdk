// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <string.h>

struct point2d {
	uint32_t x, y;
};

struct physics_object_ops {
	void (*position)(const struct physics_object_ops **ops, struct point2d *out);
};

static inline void physics_object_position(const struct physics_object_ops **ops,
					   struct point2d *out)
{
	(*ops)->position(ops, out);
}

struct server_circle {
	struct point2d pos;
	const struct physics_object_ops *ops;
};

struct server_square {
	struct point2d pos;
	const struct physics_object_ops *ops;
};

static void server_circle_position(const struct physics_object_ops **ops, struct point2d *out)
{
	struct server_circle *self = CONTAINER_OF(ops, struct server_circle, ops);

	printk("server: get position for circle (%d, %d)\n", self->pos.x, self->pos.y);
}

static void server_square_position(const struct physics_object_ops **ops, struct point2d *out)
{
	struct server_circle *self = CONTAINER_OF(ops, struct server_circle, ops);

	printk("server: get position for square (%d, %d)\n", self->pos.x, self->pos.y);
}

static const struct physics_object_ops circle_ops = { .position = server_circle_position };

static const struct physics_object_ops square_ops = { .position = &server_square_position };

void server_circle_init(struct server_circle *self)
{
	memset(self, 0, sizeof(*self));
	self->ops = &circle_ops;
}

void server_square_init(struct server_square *self)
{
	memset(self, 0, sizeof(*self));
	self->ops = &square_ops;
}

struct drawable_ops {
	void (*draw)(const struct drawable_ops **ops);
};

static inline void drawable_draw(const struct drawable_ops **ops)
{
	(*ops)->draw(ops);
}

struct client_circle {
	const struct physics_object_ops **server_object;
	const struct drawable_ops *drawable;
};

struct client_square {
	const struct physics_object_ops **server_object;
	const struct drawable_ops *drawable;
};

void client_circle_draw(const struct drawable_ops **ops)
{
	struct client_circle *self = CONTAINER_OF(ops, struct client_circle, drawable);
	struct point2d pos = { 0 };

	physics_object_position(self->server_object, &pos);
	printk("client: drawing circle at position\n");
}

void client_square_draw(const struct drawable_ops **ops)
{
	struct client_square *self = CONTAINER_OF(ops, struct client_square, drawable);
	struct point2d pos = { 0 };

	physics_object_position(self->server_object, &pos);
	printk("client: drawing square at position\n");
}

static const struct drawable_ops circle_draw_ops = { .draw = client_circle_draw };

static const struct drawable_ops square_draw_ops = { .draw = client_square_draw };

void client_circle_init(struct client_circle *self, const struct physics_object_ops **server_object)
{
	memset(self, 0, sizeof(*self));
	self->server_object = server_object;
	self->drawable = &circle_draw_ops;
}

void client_square_init(struct client_square *self, const struct physics_object_ops **server_object)
{
	memset(self, 0, sizeof(*self));
	self->server_object = server_object;
	self->drawable = &square_draw_ops;
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
	struct server_circle s_circle;
	struct server_square s_square;
	struct client_circle circle;
	struct client_square square;

	server_circle_init(&s_circle);
	server_square_init(&s_square);
	client_circle_init(&circle, &s_circle.ops);
	client_square_init(&square, &s_square.ops);
	drawable_draw(&circle.drawable);
	drawable_draw(&square.drawable);
}
