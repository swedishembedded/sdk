// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
// Consulting: https://swedishembedded.com/go
// Training: https://swedishembedded.com/tag/training

using System;
using System.Collections.Generic;
using Antmicro.Renode.Core;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Peripherals.SPI;
using Antmicro.Renode.Core.Structure.Registers;

namespace Antmicro.Renode.Peripherals.GPIOPort
{
	public class MCP23S17 : BaseGPIOPort, ISPIPeripheral, IGPIOReceiver, IBytePeripheral {
		public MCP23S17(Machine machine) : base(machine, NumberOfPins)
		{
			CS = new GPIO();
			A0 = new GPIO();
			A1 = new GPIO();
			A2 = new GPIO();
			RESET = new GPIO();
			INTB = new GPIO();
			INTA = new GPIO();

			registers = CreateRegisters();
			Reset();
		}

		public GPIO CS { get; }
		public GPIO A0 { get; }
		public GPIO A1 { get; }
		public GPIO A2 { get; }
		public GPIO RESET { get; }
		public GPIO INTB { get; }
		public GPIO INTA { get; }

		public void FinishTransmission()
		{
			byteId = 0;
			memOffset = 0;
		}

		public byte Transmit(byte data)
		{
			if (CS.IsSet)
				return 0;
			byte value = 0;
			switch (byteId) {
			case 0:
				isRead = ((data & 0x01) == 1) ? 1 : 0;
				break;
			case 1:
				memOffset = data;
				break;
			default:
				if (isRead == 1) {
					value = registers.Read(memOffset);
					if (memOffset == (uint)Registers.INTFA) {
						registers.Write(memOffset, 0);
						INTA.Set();
					} else if (memOffset == (uint)Registers.INTFB) {
						registers.Write(memOffset, 0);
						INTB.Set();
					}
				} else {
					registers.Write(memOffset, data);
				}
				memOffset++;
				break;
			}
			this.DebugLog("SPI byte {0:X}: rx: {1:X} (tx: {2:X})", byteId, data, value);
			byteId++;
			return value;
		}

		public override void Reset()
		{
			this.DebugLog("RESET");
			FinishTransmission();
			base.Reset();
			registers.Reset();
			INTA.Set();
			INTB.Set();
		}

		public byte ReadByte(long offset)
		{
			return registers.Read(offset);
		}

		public void WriteByte(long offset, byte value)
		{
			registers.Write(offset, value);
		}

		public override void OnGPIO(int number, bool value)
		{
			if (number >= 0 && number < NumberOfPins) {
				base.OnGPIO(number, value);
				Connections[number].Set(value);
				if (number < 8) {
					byte INTEN = (byte)registers.Read((long)Registers.GPINTENA);
					if ((INTEN & (byte)(1 << number)) != 0) {
						registers.Write(
							(long)Registers.INTFA,
							(byte)(registers.Read(
								       (long)Registers.INTFA) |
							       (byte)(1 << number)));
						INTA.Unset();
					} else {
						this.DebugLog(
							"Interrupt on pin {0} is not enabled in GPINTENA",
							number);
					}
				} else {
					byte INTEN = (byte)registers.Read((long)Registers.GPINTENB);
					if ((INTEN & (byte)(1 << (number - 8))) != 0) {
						registers.Write(
							(long)Registers.INTFB,
							(byte)(registers.Read(
								       (long)Registers.INTFB) |
							       (byte)(1 << (number - 8))));
						INTB.Unset();
					} else {
						this.DebugLog(
							"Interrupt on pin {0} is not enabled in GPINTENB",
							number);
					}
				}
			} else if (number == (int)GPIOPin.CS) {
				this.DebugLog("CS: {0}", value);
				CS.Set(value);
				FinishTransmission();
			}
		}

		private ByteRegisterCollection CreateRegisters()
		{
			var registersMap = new Dictionary<long, ByteRegister> {
				{ (long)Registers.IODIRA,
				  new ByteRegister(this, 0xff)
					  .WithValueField(0, 8, out IODIRA, name: "IODIRA") },
				{ (long)Registers.IODIRB,
				  new ByteRegister(this, 0xff)
					  .WithValueField(0, 8, out IODIRB, name: "IODIRB") },
				{ (long)Registers.IPOLA, new ByteRegister(this, 0).WithValueField(
								 0, 8, out IPOLA, name: "IPOLA") },
				{ (long)Registers.IPOLB, new ByteRegister(this, 0).WithValueField(
								 0, 8, out IPOLB, name: "IPOLB") },
				{ (long)Registers.GPINTENA,
				  new ByteRegister(this, 0).WithValueField(0, 8, out GPINTENA,
									   name: "GPINTENA") },
				{ (long)Registers.GPINTENB,
				  new ByteRegister(this, 0).WithValueField(0, 8, out GPINTENB,
									   name: "GPINTENB") },
				{ (long)Registers.DEFVALA,
				  new ByteRegister(this, 0).WithValueField(0, 8, out DEFVALA,
									   name: "DEFVALA") },
				{ (long)Registers.DEFVALB,
				  new ByteRegister(this, 0).WithValueField(0, 8, out DEFVALB,
									   name: "DEFVALB") },
				{ (long)Registers.INTCONA,
				  new ByteRegister(this, 0).WithValueField(0, 8, out INTCONA,
									   name: "INTCONA") },
				{ (long)Registers.INTCONB,
				  new ByteRegister(this, 0).WithValueField(0, 8, out INTCONB,
									   name: "INTCONB") },
				{ (long)Registers.IOCONA,
				  new ByteRegister(this, 0).WithValueField(0, 8, out IOCONA,
									   name: "IOCONA") },
				{ (long)Registers.IOCONB,
				  new ByteRegister(this, 0).WithValueField(0, 8, out IOCONB,
									   name: "IOCONB") },
				{ (long)Registers.GPPUA, new ByteRegister(this, 0).WithValueField(
								 0, 8, out GPPUA, name: "GPPUA") },
				{ (long)Registers.GPPUB, new ByteRegister(this, 0).WithValueField(
								 0, 8, out GPPUB, name: "GPPUB") },
				{ (long)Registers.INTFA, new ByteRegister(this, 0).WithValueField(
								 0, 8, out INTFA, name: "INTFA") },
				{ (long)Registers.INTFB, new ByteRegister(this, 0).WithValueField(
								 0, 8, out INTFB, name: "INTFB") },
				{ (long)Registers.INTCAPA,
				  new ByteRegister(this, 0).WithValueField(0, 8, out INTCAPA,
									   name: "INTCAPA") },
				{ (long)Registers.INTCAPB,
				  new ByteRegister(this, 0).WithValueField(0, 8, out INTCAPB,
									   name: "INTCAPB") },
				{ (long)Registers.GPIOA,
				  new ByteRegister(this).WithValueField(
					  0, 8, name: "GPIOA",
					  writeCallback: (_, val) => WriteOutputs(0, val),
					  valueProviderCallback: _ => ReadInputs(0))
			}, { (long)Registers.GPIOB,
			     new ByteRegister(this).WithValueField(
				     0, 8, name: "GPIOB",
				     writeCallback: (_, val) => WriteOutputs(1, val),
				     valueProviderCallback: _ => ReadInputs(1))
			},
			{(long)Registers.OLATA, new ByteRegister(this)
			 .WithValueField(0, 8, out OLATA, name: "OLATA")
			},
			{(long)Registers.OLATB, new ByteRegister(this)
			 .WithValueField(0, 8, out OLATB, name: "OLATB")
			},
		};
			return new ByteRegisterCollection(this, registersMap);
		}

		private uint ReadInputs(byte port)
		{
			uint values = BitHelper.GetValueFromBitsArray(State);
			this.DebugLog("ReadInputs port {0}: {1}", port, values);
			return (port == 0) ? (values & 0xff) : (values >> 8);
		}

		private void WriteOutputs(byte port, uint value)
		{
			if (port == 0) {
				OLATA.Value = (byte)value;
				for (var i = 0; i < 8; i++) {
					if (((value >> i) & 1) != 0) {
						State[i] = true;
						Connections[i].Set();
					} else {
						State[i] = false;
						Connections[i].Unset();
					}
				}
			} else {
				OLATB.Value = (byte)value;
				for (var i = 0; i < 8; i++) {
					if (((value >> i) & 1) != 0) {
						State[8 + i] = true;
						Connections[8 + i].Set();
					} else {
						State[8 + i] = false;
						Connections[8 + i].Unset();
					}
				}
			}
		}

		private int isRead;
		private uint byteId;
		private uint memOffset;

		private IValueRegisterField IODIRA;
		private IValueRegisterField IODIRB;
		private IValueRegisterField IPOLA;
		private IValueRegisterField IPOLB;
		private IValueRegisterField GPINTENA;
		private IValueRegisterField GPINTENB;
		private IValueRegisterField DEFVALA;
		private IValueRegisterField DEFVALB;
		private IValueRegisterField INTCONA;
		private IValueRegisterField INTCONB;
		private IValueRegisterField IOCONA;
		private IValueRegisterField IOCONB;
		private IValueRegisterField GPPUA;
		private IValueRegisterField GPPUB;
		private IValueRegisterField INTFA;
		private IValueRegisterField INTFB;
		private IValueRegisterField INTCAPA;
		private IValueRegisterField INTCAPB;
		private IValueRegisterField OLATA;
		private IValueRegisterField OLATB;

		private const int NumberOfPins = 16;
		private readonly ByteRegisterCollection registers;

		private enum GPIOPin { CS = 16 }

		private enum Registers {
			IODIRA = 0x00,
			IODIRB = 0x01,
			IPOLA = 0x02,
			IPOLB = 0x03,
			GPINTENA = 0x04,
			GPINTENB = 0x05,
			DEFVALA = 0x06,
			DEFVALB = 0x07,
			INTCONA = 0x08,
			INTCONB = 0x09,
			IOCONA = 0x0a,
			IOCONB = 0x0b,
			GPPUA = 0x0c,
			GPPUB = 0x0d,
			INTFA = 0x0e,
			INTFB = 0x0f,
			INTCAPA = 0x10,
			INTCAPB = 0x11,
			GPIOA = 0x12,
			GPIOB = 0x13,
			OLATA = 0x14,
			OLATB = 0x15,
		}
	}
}
