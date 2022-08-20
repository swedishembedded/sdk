// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 *
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 *
 * This example shows how to execute an LQI controller within the context of
 * firmware, which communicates over a socket with a simulation connected to the
 * emulator.
 *
 * It is intentionally kept very raw and without dependencies because it is
 * intended to illustrate the core concept.
 *
 * We are writing to a raw memory location which is mapped by the simulator and
 * the data we write to it is passed to an external process. We can then read
 * data back and this data is also retrieved from an external process. Thus we
 * have a simulated plant that this controller stabilizes.
 **/

#include <kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <control/controller.h>

#define ADIM 2
#define RDIM 1
#define YDIM 1
#define ANTI_WINDUP 0

/** Plant registers mapped by the simulation */
struct plant_controls {
	float u;
	float y;
	uint32_t compute;
	float r;
} __attribute__((packed)) __attribute__((aligned(4)));

static volatile struct plant_controls *plant = ((volatile struct plant_controls *)0x70000000);

#include <lqi-design.h>

void main(void)
{
	printk("Control library LQI sample\n");

	// Initial states - Normally set to zero
	float x[ADIM] = { 0 };
	float xi[1] = { 0 };

	// Input signal, reference signal, output signal and state vector x and xi
	float qi = 0.1; // <-- Tune this in between 0->1, but qi =/= 1.
	float u[RDIM] = { 0 }; // <-- This is our input value we want to have
	float r[RDIM] = { 0 }; // <-- This is what we want to see.
	float y[YDIM] = { 0 }; // <-- This is our measurement. Set it.

	while (1) {
		r[0] = plant->r;
		y[0] = plant->y;

		//Control LQI
		lqi(y, u, qi, r, L, Li, x, xi, ADIM, YDIM, RDIM, ANTI_WINDUP);

		// Estimate the state x
		kalman(A, B, C, K, u, x, y, ADIM, YDIM, RDIM);

		// Write control input to the plant
		plant->u = u[0];

		// This is used to trigger "tick" in the plant (since we are simulating)
		plant->compute = 1;

		// Print control input and output of the plant
		// The plant should stabilize at 'r'
		//printf("u = %f, y = %f\n", u[0], y[0]);
		k_sleep(K_MSEC(10));
	}
}
