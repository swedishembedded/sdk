/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <stddef.h>
#include <stdint.h>

struct opaque; // just a declaration

// init and deinit
int opaque_init(struct opaque *self);
int opaque_deinit(struct opaque *self);

// size
size_t opaque_size(void);

void opaque_set_data(struct opaque *self, uint32_t data);
uint32_t opaque_get_data(struct opaque *self);

// new and free
struct opaque *opaque_new(void);
void opaque_free(struct opaque **self);
