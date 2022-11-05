// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 *
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/

#include <zephyr/kernel.h>
#include <assert.h>
#include "plc.hpp"

using namespace std;

void tick(cxxrtl_design::p_plc &plc)
{
	plc.p_clk.set<bool>(false);
	plc.step();
	plc.p_clk.set<bool>(true);
	plc.step();
}

void main(void)
{
	cxxrtl_design::p_plc plc;

	plc.step();

	// initially everything should be off
	assert(plc.p_pump__1.get<bool>() == false);
	assert(plc.p_pump__2.get<bool>() == false);
	assert(plc.p_valve.get<bool>() == false);

	tick(plc);

	printk("! tank H_LEVEL: %d\n", plc.p_h__level.get<bool>());
	printk("! tank L_LEVEL: %d\n", plc.p_l__level.get<bool>());

	printk("=> filling up the tank\n");

	// when tank is empty, pumps must be active
	assert(plc.p_pump__1.get<bool>() == true);
	assert(plc.p_pump__2.get<bool>() == true);
	assert(plc.p_valve.get<bool>() == false);

	printk("=> tank level reached L_LEVEL\n");
	// tank has been filled to first level
	plc.p_l__level.set<bool>(true);
	tick(plc);

	printk("=> continuing filling up the tank\n");

	// tank pumps should still be active
	assert(plc.p_pump__1.get<bool>() == true);
	assert(plc.p_pump__2.get<bool>() == true);
	assert(plc.p_mixer.get<bool>() == false);
	assert(plc.p_valve.get<bool>() == false);

	// tank has been filled to upper level
	plc.p_h__level.set<bool>(true);
	tick(plc);

	printk("=> tank full\n");

	// pumps must be off
	assert(plc.p_pump__1.get<bool>() == false);
	assert(plc.p_pump__2.get<bool>() == false);
	assert(plc.p_mixer.get<bool>() == true);
	assert(plc.p_valve.get<bool>() == false);

	printk("=> mixing\n");

	for (int cycle = 0; cycle < 7000; ++cycle) {
		tick(plc);
	}

	printk("=> draining\n");

	// mixer must be off and valve should open
	assert(plc.p_pump__1.get<bool>() == false);
	assert(plc.p_pump__2.get<bool>() == false);
	assert(plc.p_mixer.get<bool>() == false);
	assert(plc.p_valve.get<bool>() == true);

	// level is down below upper
	plc.p_h__level.set<bool>(false);
	tick(plc);

	// continue to drain the tank
	assert(plc.p_pump__1.get<bool>() == false);
	assert(plc.p_pump__2.get<bool>() == false);
	assert(plc.p_mixer.get<bool>() == false);
	assert(plc.p_valve.get<bool>() == true);

	// level reached lower
	plc.p_l__level.set<bool>(false);
	tick(plc);

	printk("=> tank emptied\n");

	// pumps should start and valve is closed
	assert(plc.p_pump__1.get<bool>() == true);
	assert(plc.p_pump__2.get<bool>() == true);
	assert(plc.p_mixer.get<bool>() == false);
	assert(plc.p_valve.get<bool>() == false);

	printk("ALL CHECKS SUCCESSFUL!\n");
}
