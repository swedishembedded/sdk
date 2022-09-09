/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

#include <stdint.h>
#include <zephyr/kernel.h>

/*!
 * @defgroup example-library-api Example Library API
 * @{
 **/

/** Example object */
struct example_object {
	/** Dummy mutex for testing */
	struct k_mutex mx;
};

/**
 * \brief Initializes example object.
 * \param[in] self reference to object
 * \returns error status
 * \retval -EINVAL invalid arguments
 * \retval 0 success
 **/
int example_object_init(struct example_object *self);

/*!
 * @}
 **/
