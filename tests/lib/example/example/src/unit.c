// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include "mock_kernel.h"

#include <unity.h>
#include <example/example.h>

// this is a unit test so we will include the file directly
// this is the only place where we do this - it is the optimal solution.
#include "../../../../../lib/example/example.c"

void test_example_object_init_should_return_einval_on_invalid_args(void)
{
	TEST_ASSERT_EQUAL(-EINVAL, example_object_init(NULL));
}

void test_example(void)
{
	struct example_object ex;

	__wrap_k_mutex_init_ExpectAndReturn(&ex.mx, 0);
	TEST_ASSERT_EQUAL(0, example_object_init(&ex));
}
