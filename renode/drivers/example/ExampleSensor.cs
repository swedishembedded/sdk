// SPDX-License-Identifier: Apache-2.0
//
// Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
//
// Consulting: https://swedishembedded.com/go
// Training: https://swedishembedded.com/tag/training
//
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals.Sensor;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Peripherals.SPI;
using Antmicro.Renode.Utilities;
using System;

namespace Antmicro.Renode.Peripherals.Sensors
{
	public class ExampleSensor : ISPIPeripheral, ITemperatureSensor, IGPIOReceiver {
		public ExampleSensor()
		{
			Interrupt = new GPIO();
			Reset();
		}

		public void FinishTransmission()
		{
			Reset();
		}

		public void OnGPIO(int number, bool value)
		{
			if (number == 0 && value == true) {
				Reset();
			}
		}

		public GPIO Interrupt { get; private set; }

		public void Reset()
		{
			byteNumber = 0;
			currentSample = 0;
			register = 0;
			command = 0;
			Interrupt.Unset();
		}

		public byte Transmit(byte data)
		{
			byte value = 0;
			switch (byteNumber) {
			case 0:
				command = data;
				break;
			case 1:
				register = data;
				break;
			case 2:
				if (register == 0xAA && command == 0x01) {
					currentSample = (UInt32)(Int32)(Temperature * 10);
					value = (byte)(currentSample >> 8);
				}
				break;
			case 3:
				if (register == 0xAA && command == 0x01) {
					value = (byte)(currentSample);
				}
				break;
			}
			byteNumber++;
			return value;
		}

		public decimal Temperature
		{
			get {
				return temperature;
			}
			set {
				if (MinTemperature > value) {
					value = MinTemperature;
				} else if (value > MaxTemperature) {
					value = MaxTemperature;
				}
				temperature = value;
				Interrupt.Set();
				Interrupt.Unset();
			}
		}

		private decimal temperature;
		private uint command;
		private uint register;
		private uint currentSample;
		private uint byteNumber;

		private const decimal MaxTemperature = 150;
		private const decimal MinTemperature = -55;
	}
}
