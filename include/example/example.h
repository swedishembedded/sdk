#pragma once

#include <stdint.h>
#include <kernel.h>

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
