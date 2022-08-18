/* SPDX-License-Identifier: Apache-2.0 */
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/consulting
 * Training: https://swedishembedded.com/tag/training
 **/

#pragma once

/*!
 * @defgroup renode-library-api Renode Library API
 * @{
 **/

#include "protocol.h"

struct renode;

/**
 * \brief Create a new renode connection instance
 * \returns new instance or NULL on error
 **/
struct renode *renode_new(void);

/**
 * \brief Frees renode connection instance and sets self to NULL
 * \param self renode instance to free
 **/
void renode_free(struct renode **self);

/**
 * \brief Connect to renode
 * \param self poitner to renode instance
 * \param mainPort main connection port supplied by renode on cli
 * \param irqPort irq connection port supplied by renode on cli
 * \param ip ip address to connect to (usually 127.0.0.1)
 * \returns 0 on success or negative on error
 * \retval -EINVAL invalid argument supplied
 * \retval -ECONN connection was refused
 **/
int renode_connect(struct renode *self, int mainPort, int irqPort, const char *ip);

/**
 * \brief Close connections to renode and cleanup
 * \param self pointer to renode instance
 * \returns 0 on success, non-zero on error
 **/
int renode_disconnect(struct renode *self);

/**
 * \brief Receives a packet from renode (blocking)
 * \param self renode instance
 * \param req request received
 * \returns 0 on success and negative error on failure
 **/
int renode_wait_request(struct renode *self, struct renode_packet *req);

/**
 * \brief Sends a response packet.
 * \details Must be called after every received packet
 * \param self renode instance
 * \param res response received
 * \returns 0 on success and negative error on failure
 **/
int renode_send_response(struct renode *self, struct renode_packet *res);

/*!
 * @}
 **/
