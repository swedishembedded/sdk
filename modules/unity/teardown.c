/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2022 Swedish Embedded Consulting Group AB
 * Author: Martin Schröder <martin.schroder@swedishembedded.com>
 **/

#include <sys/printk.h>
#ifdef CONFIG_BOARD_NATIVE_POSIX
#include "posix_board_if.h"
#endif

int generic_test_suite_teardown(int num_failures)
{
	int ret = num_failures > 0 ? 1 : 0;

	printk("PROJECT EXECUTION %s\n", (num_failures) ? "FAILED" : "SUCCESSFUL");
#ifdef CONFIG_BOARD_NATIVE_POSIX
	posix_exit(ret);
#endif
	return ret;
}

__weak int __unused test_suite_teardown(int num_failures)
{
	return generic_test_suite_teardown(num_failures);
}
