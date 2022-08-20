/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/
#pragma once

/*!
 * @defgroup example-driver-api Example Driver API
 * @{
 **/

/**
 * \brief Example driver public function
 * \param[in] dev reference to device
 * \returns error status
 * \retval -EINVAL invalid arguments
 * \retval 0 success
 **/
int example_driver_function(const struct device *dev);

/*!
 * @}
 **/
