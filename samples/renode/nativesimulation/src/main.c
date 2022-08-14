// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

#include <kernel.h>
#include <device.h>
#include <project_version.h>

#define REG_BASE ((volatile uint32_t *)0x70000000)
#define REG_IN *(REG_BASE + 0x01)
#define REG_OUT *(REG_BASE + 0x02)

void main(void)
{
	printk("Native Simulation Sample (v%s)\n", PROJECT_VERSION_STRING);

	// this is peripheral address
	printk("Previous return value (%p) = %08x\n", &REG_OUT, REG_OUT);
	REG_IN = 555;
	printk("New return value = %08x\n", REG_OUT);
};
