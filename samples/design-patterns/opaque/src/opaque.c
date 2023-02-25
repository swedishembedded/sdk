// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include "opaque.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// actual definition of the struct in private space of the c file
struct opaque {
	uint32_t data;
};

int opaque_init(struct opaque *self)
{
	memset(self, 0, sizeof(*self));
	return 0;
}

int opaque_deinit(struct opaque *self)
{
	// free any internal resources
	self->data = 0;
	return 0;
}

size_t opaque_size(void)
{
	return sizeof(struct opaque);
}

void opaque_set_data(struct opaque *self, uint32_t data)
{
	self->data = data;
}

uint32_t opaque_get_data(struct opaque *self)
{
	return self->data;
}

struct opaque *opaque_new(void)
{
	return malloc(sizeof(struct opaque));
}

void opaque_free(struct opaque **self)
{
	free(*self);
	*self = NULL;
}
