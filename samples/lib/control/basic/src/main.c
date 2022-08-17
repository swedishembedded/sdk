// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
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
#include <stdio.h>
#include <control/controller.h>

#define ADIM 2
#define RDIM 1
#define YDIM 1
#define ANTI_WINDUP 0

enum plant_register {
	PLANT_REG_U = 0,
	PLANT_REG_Y = 1,
	PLANT_REG_COMPUTE = 2,
	PLANT_REG_R = 3,
};

#define PLANT_BASE ((volatile uint32_t *)0x70000000)
#define PLANT_Y0 *(PLANT_BASE + PLANT_REG_Y)
#define PLANT_U0 *(PLANT_BASE + PLANT_REG_U)
#define PLANT_COMPUTE *(PLANT_BASE + PLANT_REG_COMPUTE)
#define PLANT_R *(PLANT_BASE + PLANT_REG_R)

void main(void)
{
	printk("Control library LQI sample\n");

	// State transition matrix of the plant
	float A[ADIM * ADIM] = { 0.89559, 0.37735, -0.37735, 0.51825 };
	// Input transition matrix
	float B[ADIM * RDIM] = { 0.20881, 0.75469 };
	// Plant output transition matrix
	float C[YDIM * ADIM] = { 1, 0 };

	// Kalman steady state matrix K
	float K[ADIM * YDIM] = { 0.58006, -0.22391 };

	// Control law L
	float L[RDIM * ADIM] = { 1.56766, 0.85103 };

	// Integral law Li
	float Li[RDIM] = { 0.50135 };

	// Initial states - Normally set to zero
	float x[ADIM] = { 0 };
	float xi[1] = { 0 };

	// Input signal, reference signal, output signal and state vector x and xi
	float qi = 0.1; // <-- Tune this in between 0->1, but qi =/= 1.
	float u[RDIM] = { 0 }; // <-- This is our input value we want to have
	float r[RDIM] = { 10 }; // <-- This is what we want to see.
	float y[YDIM] = { 0 }; // <-- This is our measurement. Set it.

	while (1) {
		//Control LQI
		lqi(y, u, qi, r, L, Li, x, xi, ADIM, YDIM, RDIM, ANTI_WINDUP);

		// Estimate the state x
		kalman(A, B, C, K, u, x, y, ADIM, YDIM, RDIM);

		// Write control input to the plant
		PLANT_U0 = (uint32_t)(u[0] * 0xFFFF);
		// Read measurement from the plant
		y[0] = ((float)PLANT_Y0 / 0xFFFF);
		r[0] = ((float)PLANT_R / 0xFFFF);

		// This is used to trigger "tick" in the plant (since we are simulating)
		PLANT_COMPUTE = 1;

		// Print control input and output of the plant
		// The plant should stabilize at 'r'
		//printf("u = %f, y = %f\n", u[0], y[0]);
		k_sleep(K_MSEC(10));
	}
}
