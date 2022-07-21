#pragma once

#include <stdint.h>

/** Example object */
struct example_object {
	/** Dummy variable */
	uint8_t dummy;
};

/**
 * \brief Initializes example object.
 * \param[in] self reference to object
 * \returns error status
 * \retval -EINVAL invalid arguments
 * \retval 0 success
 **/
int example_object_init(struct example_object *self);
