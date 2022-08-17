/* SPDX-License-Identifier: Apache-2.0
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Concurrent;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using Antmicro.Renode.Core;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Peripherals.Bus;
using Antmicro.Renode.Peripherals.CPU;
using Antmicro.Renode.Peripherals.Timers;
using Antmicro.Renode.Utilities.Binding;
using Antmicro.Renode.Debugging;
using Antmicro.Renode.Logging;

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
	Reset,
	/** Interrupt notification from peripheral side */
	Interrupt,
	/** Error response from peripheral side */
	Error,
	/** Acknowledge/ok response */
	OK,
	/** Disconnection message */
	Disconnect,
	/** Handshake message */
	Handshake,
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

public class SocketPeripheral : IDoubleWordPeripheral, IDisposable, INumberedGPIOOutput {
	/** Message received from the shared library */
	private ProtocolMessage? responseMessage;
	private Process peripheralProcess;
	private SocketComunicator mainSocket;
	private SocketComunicator irqSocket;
	/** Lock for native calls */
	public IReadOnlyDictionary<int, IGPIO> Connections { get; }
	private string peripheralFilePath = "peripheral.elf";
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
	private readonly string address = "127.0.0.1";

	public SocketPeripheral(Machine machine)
	{
		mainSocket = new SocketComunicator(this, timeout, this.address);
		irqSocket = new SocketComunicator(this, Timeout.Infinite, this.address);
		respReceived = new AutoResetEvent(initialState: false);
		peripheralActive = new CancellationTokenSource();
		irqMonitor = new Thread(IRQMonitor) { IsBackground = true,
						      Name = "SocketPeripheral.IRQMonitor" };
		irqMonitor.Start();
		this.Log(LogLevel.Info, "Listening on ports {0} {1}", mainSocket.ListenerPort, irqSocket.ListenerPort);
	}

	/** Shared library path */
	public string PeripheralPath
	{
		get {
			return peripheralFilePath;
		}
		set {
			try {
				peripheralFilePath = value;
				InitPeripheralProcess(value, mainSocket.ListenerPort,
						      irqSocket.ListenerPort);
				this.Log(LogLevel.Info, "Connecting to peripheral {0}", value);

				if (!mainSocket.AcceptConnection(timeout) ||
				    !irqSocket.AcceptConnection(timeout) || !TryHandshake()) {
					mainSocket.ResetConnections();
					irqSocket.ResetConnections();
					KillPeripheralProcess();

					this.Log(LogLevel.Error,
						$"Connection to the socket peripheral ({value}) failed!");
				} else {
					// If connected successfully, listening sockets can be closed
					mainSocket.CloseListener();
					irqSocket.CloseListener();

					this.Log(LogLevel.Debug,
							  "Connected to the verilated peripheral!");
				}
			} catch (Exception e) {
				this.Log(LogLevel.Error, "Error starting peripheral {0}", e);
				throw new RecoverableException(e);
			}
		}
	}

	private void InitPeripheralProcess(string filePath, int mainPort, int receiverPort)
	{
		try {
			peripheralProcess =
				new Process { StartInfo = new ProcessStartInfo(filePath) {
					UseShellExecute = false,
					Arguments = $"{mainPort} {receiverPort} {address}"
				} };

			peripheralProcess.Start();
		} catch (Exception e) {
			peripheralProcess = null;
			this.Log(LogLevel.Error, $"Error starting peripheral process {e}");
		}
	}

	private void KillPeripheralProcess()
	{
		try {
			peripheralProcess?.Kill();
		} catch {
			return;
		}
	}

	private bool TryHandshake()
	{
		if(Send(new ProtocolMessage(MessageType.Handshake, 0, 0)) &&
			 Recv(out var result) && result.Type == MessageType.Handshake){
			return true;
		}
		this.Log(LogLevel.Error, "No handshake received");
		return false;
	}

	private bool Send(ProtocolMessage message)
	{
		return mainSocket.TrySendMessage(message);
	}

	private bool Recv(out ProtocolMessage message)
	{
		return mainSocket.TryReceiveMessage(out message);
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
			Send(new ProtocolMessage(MessageType.WriteToBus, (ulong)offset / 4, value));
			if (Recv(out var msg) && msg.Type == MessageType.OK)
				return;
			this.Log(LogLevel.Error, "Failed to write data");
	}

	public void Reset()
	{
		Send(new ProtocolMessage(MessageType.Reset, 0, 0));
		Recv(out var msg);
	}

	public void Dispose()
	{
		irqSocket.CancelCommunication();
		//pauseMRES?.Dispose();
		if (peripheralProcess != null) {
			// Ask peripheralProcess to close, kill if it doesn't
			if (!peripheralProcess.HasExited) {
				this.DebugLog(
					$"Verilated peripheral '{peripheralFilePath}' is still working...");
				var exited = false;

				if (mainSocket.Connected) {
					this.DebugLog(
						"Trying to close it gracefully by sending 'Disconnect' message...");
					Send(
						new ProtocolMessage(MessageType.Disconnect, 0, 0));
					mainSocket.CancelCommunication();
					exited = peripheralProcess.WaitForExit(500);
				}

				if (exited) {
					this.DebugLog(
						"Verilated peripheral exited gracefully.");
				} else {
					KillPeripheralProcess();
					this.Log(LogLevel.Warning,
							  "Verilated peripheral had to be killed.");
				}
			}
			peripheralProcess.Dispose();
		}

		mainSocket.Dispose();
		irqSocket.Dispose();

		peripheralActive.Cancel();
	}

	private void IRQMonitor()
	{
		this.Log(LogLevel.Debug, "IRQMonitor init");
		try {
			while (!peripheralActive.Token.IsCancellationRequested) {
				//var message = irqMessages.Take(peripheralActive.Token);
				//this.Log(LogLevel.Debug, "IRQ message received");
				//HandleIRQ(message);
			}
		} catch (OperationCanceledException) {
			this.Log(LogLevel.Info, "IRQMonitor Cancelled!");
			return;
		}
	}

	private class SocketComunicator {
		public SocketComunicator(IEmulationElement logger, int timeoutInMilliseconds,
					 string address)
		{
			disposalCTS = new CancellationTokenSource();
			channelTaskFactory = new TaskFactory<int>(disposalCTS.Token);
			this.logger = logger;
			this.address = address;
			timeout = timeoutInMilliseconds;
			ListenerPort = CreateListenerAndStartListening();
		}

		public void Dispose()
		{
			listener?.Close(timeout);
			socket?.Close(timeout);
			disposalCTS.Dispose();
		}

		public bool AcceptConnection(int timeoutInMilliseconds)
		{
			// Check if there's any connection waiting to be accepted (with timeout in MICROseconds)
			var acceptAttempt =
				listener.Poll(timeoutInMilliseconds * 1000, SelectMode.SelectRead);
			if (acceptAttempt) {
				socket = listener.Accept();
			}
			return acceptAttempt;
		}

		public void CloseListener()
		{
			listener.Close();
			listener = null;
		}

		public void ResetConnections()
		{
			socket?.Close();

			if (listener.Poll(0, SelectMode.SelectRead)) {
				logger.DebugLog(
					$"Clients are pending on the listening {ListenerPort} port. Connection queue will be reset.");

				// There's no other way to reset listener's connection queue
				CloseListener();
				ListenerPort = CreateListenerAndStartListening();
			}
		}

		public void CancelCommunication()
		{
			disposalCTS.Cancel();
		}

		public bool TrySendMessage(ProtocolMessage message)
		{
			var serializedMessage = message.Serialize();
			var size = serializedMessage.Length;
			var task = channelTaskFactory.FromAsync(
				(callback, state) =>
					socket.BeginSend(serializedMessage, 0, size,
							 SocketFlags.None, callback, state),
				socket.EndSend, state: null);

			return WaitSendOrReceiveTask(task, size);
		}

		public bool TryReceiveMessage(out ProtocolMessage message)
		{
			message = default(ProtocolMessage);

			var result = TryReceive(out var buffer, Marshal.SizeOf(message));
			if (result) {
				message.Deserialize(buffer);
			}
			return result;
		}

		public bool TryReceiveString(out string message, int size)
		{
			message = String.Empty;
			var result = TryReceive(out var buffer, size);
			if (result) {
				message = Encoding.ASCII.GetString(buffer);
			}
			return result;
		}

		public bool TryReceive(out byte[] buffer, int size)
		{
			buffer = null;
			var taskBuffer = new byte[size];
			var task = channelTaskFactory.FromAsync(
				(callback, state) => socket.BeginReceive(
					taskBuffer, 0, size, SocketFlags.None, callback, state),
				socket.EndReceive, state: null);

			var isSuccess = WaitSendOrReceiveTask(task, size);
			if (isSuccess) {
				buffer = taskBuffer;
			}
			return isSuccess;
		}

		public int ListenerPort { get; private set; }
		public bool Connected => socket.Connected;

		private int CreateListenerAndStartListening()
		{
			listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream,
					      ProtocolType.Tcp);
			listener.Bind(new IPEndPoint(IPAddress.Parse(address), 0));

			listener.Listen(MaxPendingConnections);
			return (listener.LocalEndPoint as IPEndPoint).Port;
		}

		private bool WaitSendOrReceiveTask(Task<int> task, int size)
		{
			try {
				task.Wait(timeout);
			}
			// Exceptions thrown from the task are always packed in AggregateException
			catch (AggregateException aggregateException) {
				foreach (var innerException in aggregateException.InnerExceptions) {
					logger.DebugLog("Send/Receive task exception: {0}",
							innerException.Message);
				}
			}

			if (task.Status != TaskStatus.RanToCompletion || task.Result != size) {
				if (task.Status == TaskStatus.Canceled) {
					logger.DebugLog(
						"Send/Receive task canceled (e.g. due to removing the peripheral).");
				} else {
					logger.DebugLog("Error while trying to Send/Receive!");
				}
				return false;
			}
			return true;
		}

		private Socket listener;
		private Socket socket;

		private readonly int timeout;
		private readonly string address;
		private readonly CancellationTokenSource disposalCTS;
		private readonly TaskFactory<int> channelTaskFactory;
		private readonly IEmulationElement logger;
		private const int MaxPendingConnections = 1;
	}
}
}
