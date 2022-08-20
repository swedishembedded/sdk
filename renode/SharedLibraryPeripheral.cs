/* SPDX-License-Identifier: Apache-2.0
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 * Consulting: https://swedishembedded.com/go
 * Training: https://swedishembedded.com/tag/training
 **/
using System;
using System.Threading;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Concurrent;
using Antmicro.Renode.Core;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Peripherals.CPU;
using Antmicro.Renode.Peripherals.Timers;
using Antmicro.Renode.Utilities.Binding;
using Antmicro.Renode.Debugging;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals;

namespace Antmicro.Renode.Peripherals
{
// MessageType must be in sync with the Verilator integration library
public enum MessageType {
	InvalidAction = 0,
	/** Synchronous tick of peripheral clock */
	TickClock,
	/** Write data to the peripheral */
	WriteToBus,
	/** Read data from the peripheral */
	ReadFromBus,
	/** Reset peripheral into initial state */
	ResetPeripheral,
	/** Interrupt notification from peripheral side */
	Interrupt,
	/** Error response from peripheral side */
	Error,
	/** Acknowledge/ok response */
	OK,
}

/** Communication protocol with the shared library */
[StructLayout(LayoutKind.Sequential, Pack = 2)]
public struct ProtocolMessage {
	public ProtocolMessage(MessageType typeId, ulong address, ulong data)
	{
		this.Type = typeId;
		this.Address = address;
		this.Data = data;
	}

	public byte[] Serialize()
	{
		var size = Marshal.SizeOf(this);
		var result = new byte[size];
		var handler = default(GCHandle);

		try {
			handler = GCHandle.Alloc(result, GCHandleType.Pinned);
			Marshal.StructureToPtr(this, handler.AddrOfPinnedObject(), false);
		} finally {
			if (handler.IsAllocated) {
				handler.Free();
			}
		}

		return result;
	}

	public void Deserialize(byte[] message)
	{
		var handler = default(GCHandle);
		try {
			handler = GCHandle.Alloc(message, GCHandleType.Pinned);
			this = (ProtocolMessage)Marshal.PtrToStructure(handler.AddrOfPinnedObject(),
								       typeof(ProtocolMessage));
		} finally {
			if (handler.IsAllocated) {
				handler.Free();
			}
		}
	}

	public MessageType Type { get; set; }
	public ulong Address { get; set; }
	public ulong Data { get; set; }
}

public class SharedLibraryPeripheral : IDoubleWordPeripheral, IDisposable, INumberedGPIOOutput {
	/** Message received from the shared library */
	private ProtocolMessage? responseMessage;
	/** Lock for native calls */
	private readonly object nativeLock;
	private IntPtr responseMessageRaw;
	private IntPtr irqMessageRaw;
	private NativeBinder binder;
	public IReadOnlyDictionary<int, IGPIO> Connections { get; }
	private string libraryFilePath;
	private readonly AutoResetEvent respReceived;
	private readonly BlockingCollection<ProtocolMessage> txQueue;
	/** Cancellation token for waiting on queues */
	private readonly CancellationTokenSource peripheralActive;
	/** IRQ monitoring thread */
	private readonly Thread irqMonitor;
	/** Asynchronous messages queue */
	private readonly BlockingCollection<ProtocolMessage> irqMessages;
	/** Timeout for reception */
	private readonly int timeout = 5000;

	public SharedLibraryPeripheral(Machine machine)
	{
		respReceived = new AutoResetEvent(initialState: false);
		peripheralActive = new CancellationTokenSource();
		irqMessages = new BlockingCollection<ProtocolMessage>();
		txQueue = new BlockingCollection<ProtocolMessage>();
		irqMonitor = new Thread(IRQMonitor) { IsBackground = true,
						      Name = "SharedLibrary.IRQMonitor" };
		nativeLock = new object();
	}

	/** Shared library path */
	public string LibraryPath
	{
		get {
			return libraryFilePath;
		}
		set {
			lock (nativeLock)
			{
				try {
					libraryFilePath = value;
					// Load shared library and bind it to methods in this class
					binder = new NativeBinder(this, value);
					// Call native initialization routine (defined inside the shared library)
					initializeNative();
					responseMessageRaw = Marshal.AllocHGlobal(
						Marshal.SizeOf(typeof(ProtocolMessage)));
					irqMessageRaw = Marshal.AllocHGlobal(
						Marshal.SizeOf(typeof(ProtocolMessage)));
					// Call shared library reset method
					resetPeripheral();
					irqMonitor.Start();
				} catch (Exception e) {
					this.Log(LogLevel.Error, "Error starting peripheral {0}",
						 e);
					throw new RecoverableException(e);
				}
			}
		}
	}

	private void Send(ProtocolMessage message)
	{
		lock (nativeLock)
		{
			Marshal.StructureToPtr(message, responseMessageRaw, true);
			handleRequest(responseMessageRaw);
		}
	}

	private bool Recv(out ProtocolMessage message)
	{
		if (respReceived.WaitOne(timeout)) {
			message = responseMessage.Value;
			responseMessage = null;
			return true;
		}

		message = default(ProtocolMessage);
		return false;
	}

	public uint ReadDoubleWord(long offset)
	{
		Send(new ProtocolMessage(MessageType.ReadFromBus, (ulong)offset / 4, 0));
		if (Recv(out var message)) {
			this.Log(LogLevel.Debug, "Received message {0} {1} {2}", message.Type,
				 message.Address, message.Data);
			return (uint)(long)message.Data;
		}
		return 0;
	}

	public void WriteDoubleWord(long offset, uint value)
	{
		lock (nativeLock)
		{
			Send(new ProtocolMessage(MessageType.WriteToBus, (ulong)offset / 4, value));
			if (Recv(out var msg) && msg.Type == MessageType.OK)
				return;
			this.Log(LogLevel.Error, "Failed to write data");
		}
	}

	public void Reset()
	{
		lock (nativeLock)
		{
			resetPeripheral();
		}
	}

	public void Dispose()
	{
		peripheralActive.Cancel();
		binder?.Dispose();
		Marshal.FreeHGlobal(responseMessageRaw);
		Marshal.FreeHGlobal(irqMessageRaw);
	}

	private void IRQMonitor()
	{
		this.Log(LogLevel.Debug, "IRQMonitor init");
		try {
			while (!peripheralActive.Token.IsCancellationRequested) {
				var message = irqMessages.Take(peripheralActive.Token);
				this.Log(LogLevel.Debug, "IRQ message received");
				//HandleIRQ(message);
			}
		} catch (OperationCanceledException) {
			this.Log(LogLevel.Info, "IRQMonitor Cancelled!");
			return;
		}
	}

	[Export]
	public void HandleResponse(IntPtr received)
	{
		// Main is used when Renode initiates communication.
		responseMessage =
			(ProtocolMessage)Marshal.PtrToStructure(received, typeof(ProtocolMessage));
		this.Log(LogLevel.Debug, "Message received {0}", (long)responseMessage.Value.Data);
		respReceived.Set();
	}

	[Export]
	public void HandleIRQMessage(IntPtr received)
	{
		// Sender is used when peripheral initiates communication.
		try {
			var message = (ProtocolMessage)Marshal.PtrToStructure(
				received, typeof(ProtocolMessage));
			irqMessages.Add(message, peripheralActive.Token);
		} catch (OperationCanceledException e) {
			this.Log(LogLevel.Error, "HandleIRQMessage error: {0}", e);
			return;
		}
	}

	[Export]
	public void Receive(IntPtr messagePtr)
	{
		try {
			var message = txQueue.Take(peripheralActive.Token);
			Marshal.StructureToPtr(message, messagePtr, false);
		} catch (OperationCanceledException) {
			return;
		}
	}

#pragma warning disable 649
	[Import(UseExceptionWrapper = false)]
	private ActionIntPtr handleRequest;
	[Import(UseExceptionWrapper = false)]
	private Action initializeNative;
	[Import(UseExceptionWrapper = false)]
	public Action resetPeripheral;
#pragma warning restore 649
}
}
