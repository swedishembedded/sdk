// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <unity.h>
#include <sys/mock_socket.h>
#include <arpa/mock_inet.h>

int close(int sock);
#include "../../../../../lib/renode/renode.c"

void test_renode_new_free(void)
{
	struct renode *s = renode_new();

	TEST_ASSERT_NOT_NULL(s);
	renode_free(&s);
	TEST_ASSERT_NULL(s);
}

void test_renode_connect_should_return_econnrefused_on_socket_fail(void)
{
	struct renode *s = renode_new();
	const char *addr = "0.0.0.0";

	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, -1);
	TEST_ASSERT_EQUAL(-ECONNREFUSED, renode_connect(s, 123, 456, addr));
	TEST_ASSERT_EQUAL(s->mainSocket, -1);
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 11);
	__wrap_inet_pton_ExpectAndReturn(AF_INET, addr, NULL, 0);
	__wrap_inet_pton_IgnoreArg_dst();
	__wrap_connect_ExpectAndReturn(11, NULL, sizeof(struct sockaddr_in), -1);
	__wrap_connect_IgnoreArg_addr();
	TEST_ASSERT_EQUAL(-ECONNREFUSED, renode_connect(s, 123, 456, addr));
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 11);
	__wrap_inet_pton_ExpectAndReturn(AF_INET, addr, NULL, 0);
	__wrap_inet_pton_IgnoreArg_dst();
	__wrap_connect_ExpectAndReturn(11, NULL, sizeof(struct sockaddr_in), 0);
	__wrap_connect_IgnoreArg_addr();
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, -1);
	TEST_ASSERT_EQUAL(-ECONNREFUSED, renode_connect(s, 123, 456, addr));
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 11);
	__wrap_inet_pton_ExpectAndReturn(AF_INET, addr, NULL, 0);
	__wrap_inet_pton_IgnoreArg_dst();
	__wrap_connect_ExpectAndReturn(11, NULL, sizeof(struct sockaddr_in), 0);
	__wrap_connect_IgnoreArg_addr();
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 22);
	__wrap_connect_ExpectAndReturn(22, NULL, sizeof(struct sockaddr_in), -1);
	__wrap_connect_IgnoreArg_addr();
	TEST_ASSERT_EQUAL(-ECONNREFUSED, renode_connect(s, 123, 456, addr));
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 11);
	__wrap_inet_pton_ExpectAndReturn(AF_INET, addr, NULL, 0);
	__wrap_inet_pton_IgnoreArg_dst();
	__wrap_connect_ExpectAndReturn(11, NULL, sizeof(struct sockaddr_in), 0);
	__wrap_connect_IgnoreArg_addr();
	__wrap_socket_ExpectAndReturn(AF_INET, SOCK_STREAM, 0, 22);
	__wrap_connect_ExpectAndReturn(22, NULL, sizeof(struct sockaddr_in), 0);
	__wrap_connect_IgnoreArg_addr();
	TEST_ASSERT_EQUAL(0, renode_connect(s, 123, 456, addr));
	renode_free(&s);
}

void test_renode_disconnect_should_return_zero(void)
{
	struct renode *s = renode_new();

	TEST_ASSERT_EQUAL(0, renode_disconnect(s));
	renode_free(&s);
}

void test_renode_wait_request_should_return_eio_on_io_fail(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_recv_ExpectAndReturn(0, &req, sizeof(req), 0, -1);
	TEST_ASSERT_EQUAL(-EIO, renode_wait_request(s, &req));
	renode_free(&s);
}

void test_renode_wait_request_should_return_einval_on_wrong_read_length(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_recv_ExpectAndReturn(0, &req, sizeof(req), 0, 0);
	TEST_ASSERT_EQUAL(-EINVAL, renode_wait_request(s, &req));

	renode_free(&s);
}

void test_renode_wait_request_should_return_zero_on_success(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_recv_ExpectAndReturn(0, &req, sizeof(req), 0, sizeof(req));
	TEST_ASSERT_EQUAL(0, renode_wait_request(s, &req));

	renode_free(&s);
}

void test_renode_send_response_should_return_eio_on_io_fail(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, &req, sizeof(req), 0, -1);
	TEST_ASSERT_EQUAL(-EIO, renode_send_response(s, &req));

	renode_free(&s);
}

void test_renode_send_response_should_return_einval_on_wrong_write_length(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, &req, sizeof(req), 0, 0);
	TEST_ASSERT_EQUAL(-EINVAL, renode_send_response(s, &req));

	renode_free(&s);
}

void test_renode_send_response_should_return_zero_on_success(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, &req, sizeof(req), 0, sizeof(req));
	TEST_ASSERT_EQUAL(0, renode_send_response(s, &req));

	renode_free(&s);
}

void test_renode_irq_notify_should_return_eio_on_io_fail(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, NULL, sizeof(req), 0, -1);
	__wrap_send_IgnoreArg_buf();
	TEST_ASSERT_EQUAL(-EIO, renode_irq_notify(s));

	renode_free(&s);
}

void test_renode_irq_notify_should_return_einval_on_wrong_write_length(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, NULL, sizeof(req), 0, 0);
	__wrap_send_IgnoreArg_buf();
	TEST_ASSERT_EQUAL(-EINVAL, renode_irq_notify(s));

	renode_free(&s);
}

void test_renode_irq_notify_should_return_zero_on_success(void)
{
	struct renode *s = renode_new();
	struct renode_packet req;

	__wrap_send_ExpectAndReturn(0, NULL, sizeof(req), 0, sizeof(req));
	__wrap_send_IgnoreArg_buf();
	TEST_ASSERT_EQUAL(0, renode_irq_notify(s));

	renode_free(&s);
}
