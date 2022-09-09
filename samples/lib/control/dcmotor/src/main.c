// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 */

#include <control/dynamics.h>

#include <float.h>

#include <instruments/dcmotor.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

static volatile struct dcmotor_instrument *vdev =
	((volatile struct dcmotor_instrument *)0x70000000);

#define ADC_NUM_CHANNELS 1
#define CONTROLLER_PID 0
#define CONTROLLER_LQI 1

/* Get the numbers of up to two channels */
static uint8_t channel_ids[ADC_NUM_CHANNELS] = {
	0,
};

struct adc_channel_cfg channel_cfg = { .gain = ADC_GAIN_1,
				       .reference = ADC_REF_INTERNAL,
				       .acquisition_time = ADC_ACQ_TIME_DEFAULT,
				       .channel_id = 0,
				       .differential = 0 };

static int16_t sample_buffer[ADC_NUM_CHANNELS];

struct adc_sequence sequence = {
	.channels = 0,
	.buffer = sample_buffer,
	.buffer_size = sizeof(sample_buffer),
	.resolution = 12,
};

#define IRQ_PIN 8

struct application {
	struct gpio_callback irq_cb;
};

static struct application app;

static void irq_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	(void)dev;
	(void)cb;
	(void)pins;
	// read interrupt flags
	uint32_t intf = vdev->INTF;

	printk("IRQ triggered (INTF: %08x)\n", intf);
}

void main(void)
{
	int ret;

	(void)channel_ids;

	printk("Renode interrupt handling sample\n");

	const struct device *pwm = device_get_binding("PWM_TIM1");

	if (!device_is_ready(pwm)) {
		printk("Error: PWM device %s is not ready\n", pwm->name);
		return;
	}

	ret = pwm_set(pwm, 1, 1000, 500, 0);
	if (ret < 0) {
		printk("Error %d: failed to set pulse width\n", ret);
		return;
	}

	const struct device *dev = device_get_binding("GPIOB");

	if (!dev) {
		printk("Could not get GPIO\n");
		return;
	}

	if (gpio_pin_configure(dev, IRQ_PIN, GPIO_INPUT) != 0) {
		printk("Error initializing INTA gpio\n");
		return;
	}
	if (gpio_pin_interrupt_configure(dev, IRQ_PIN, GPIO_INT_EDGE_TO_ACTIVE) != 0) {
		printk("Error configuring interrupt on INTA gpio\n");
		return;
	}

	gpio_init_callback(&app.irq_cb, irq_handler, BIT(IRQ_PIN));
	if (gpio_add_callback(dev, &app.irq_cb) != 0) {
		printk("Error setting callback for GPIO\n");
		return;
	}

	//struct pid pid;
#define ADIM 2
#define RDIM 1
#define YDIM 1

	float A[ADIM * ADIM] = { 0.989936f, 0.082495f, -0.001650f, 0.833196f };
	//float A[ADIM * ADIM] = { 0.99, 0.09016, -0.001803, 0.8186 };
	//float B[ADIM * RDIM] = { 0.009334, 0.1813 };
	float B[ADIM * RDIM] = { 0.016499f, 0.166639f };
	float C[YDIM * ADIM] = { 1, 0 };
	float K[ADIM * YDIM] = { 0.106218f, 0.052683f };
	//float L[RDIM * ADIM] = { 2.4522, 1.0691 };

	float x[ADIM] = { 0, 0 };
	float xi[1] = { 0 };

	struct pid pid;

	pid_init(&pid);

	while (1) {
		float u[RDIM] = { 0 };
		//float r[RDIM] = { 0 };
		float y[YDIM] = { -(vdev->reference - vdev->omega) };

		if (vdev->controller == CONTROLLER_PID) {
			pid_set_gains(&pid, vdev->pid.Kp, vdev->pid.Ki, vdev->pid.Kd, vdev->pid.d);
			u[0] = -(vdev->Kff * y[0] + pid_step(&pid, y[0]));
		} else if (vdev->controller == CONTROLLER_LQI) {
			float L[RDIM * ADIM] = { vdev->lqi.L[0], vdev->lqi.L[1] };
			float Li[RDIM] = { vdev->lqi.Li };

			//Control LQI
			lqi(u, L, x, Li, xi, y, ADIM, YDIM, RDIM, 0);
			u[0] -= (vdev->Kff * y[0]);
		}

		// output limiting
		if (u[0] > 25)
			u[0] = 25;
		if (u[0] < -25)
			u[0] = -25;

		// send control input to the plant
		vdev->control = u[0];

		// Execute state estimator regardless of controller
		kalman(x, A, x, B, u, K, y, C, ADIM, YDIM, RDIM);

		vdev->tick = 1;
		k_sleep(K_MSEC(10));
	}
}
