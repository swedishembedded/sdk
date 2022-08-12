# SPDX-License-Identifier: Apache-2.0
import os
import clr
import sys
import SimpleHTTPServer
import SocketServer
from threading import Thread

CWD = os.path.realpath(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(CWD)
os.chdir(CWD)

from websocket_server import WebsocketServer

clr.AddReference("Renode-peripherals")
clr.AddReference("IronPython.StdLib")

import Antmicro
from Antmicro import Renode
from Antmicro.Renode.Peripherals.Miscellaneous import LED
from Antmicro.Renode.Peripherals.Miscellaneous import Button
from Antmicro.Renode.Peripherals.UART import IUART

PROJECT_BASE = CWD + "/../../"


def send_message(message):
    if websocket_server is not None and len(websocket_server.clients) > 0:
        for client in websocket_server.clients:
            try:
                websocket_server.send_message(client, message)
            except Exception:
                # in case something got disconnected
                pass


# called when a client sends a message
def on_message(client, server, message):
    emulation = Antmicro.Renode.Core.EmulationManager.Instance.CurrentEmulation
    machine = emulation.Machines[0]
    split = message.split("|")
    if split[0] == "BTN":
        btn = None
        try:
            btn = clr.Convert(
                machine["sysbus.spi1.gpioExpander." + split[1]],
                Renode.Peripherals.Miscellaneous.Button,
            )
        except BaseException:
            print("Peripheral not found or not an Button: %s" % (split[1]))
            return 1

        print("BTN: %s" % message)
        if split[2] == "Press":
            btn.Press()
        else:
            btn.Release()
    else:
        if len(message) > 200:
            message = message[:200] + ".."
        print("Client(%d) said: %s" % (client["id"], message))


def on_led(led, state):
    ev = ["LED", machine.GetLocalName(led), str(state)]
    print("LED: %s" % ev)
    send_message("|".join(ev))


def on_uart_chars(char, uartName):
    send_message("|".join(["UART", uartName, chr(char)]))


main_server = None
server_thread = None
websocket_server = None
websocket_thread = None
uart_event_map = {}
machine = None

print("Adding serveVisualization command.")


def mc_startVisual(port):
    global main_server
    global server_thread
    global websocket_server
    global websocket_thread
    global machine

    # get emulation
    emulation = Antmicro.Renode.Core.EmulationManager.Instance.CurrentEmulation

    # Assume a single machine for now.
    machine = emulation.Machines[0]

    leds = machine.GetPeripheralsOfType[LED]()
    for led in leds:
        led.StateChanged += on_led

    btns = machine.GetPeripheralsOfType[Button]()
    for btn in btns:
        btn.Press()

    uarts = machine.GetPeripheralsOfType[IUART]()
    for uart in uarts:
        uartName = clr.Reference[str]()
        if not machine.TryGetAnyName(uart, uartName):
            on_received = lambda char: on_uart_chars(char, uartName)
        else:
            on_received = lambda char: on_uart_chars(char, "unknown_uart")
        uart.CharReceived += on_received
        uart_event_map[uart] = on_received

    # setting up server threads
    websocket_server = WebsocketServer(9001)
    websocket_server.set_fn_message_received(on_message)
    websocket_thread = Thread(target=websocket_server.serve_forever)
    websocket_thread.daemon = True
    websocket_thread.start()

    try:
        main_server = SocketServer.TCPServer(
            ("", port), SimpleHTTPServer.SimpleHTTPRequestHandler
        )
        server_thread = Thread(target=main_server.serve_forever)
        server_thread.daemon = True

        print("Serving interactive visualization at port", port)
        server_thread.start()
    except Exception:
        import traceback

        traceback.print_exc()


def mc_stopVisual():
    global main_server
    global server_thread
    global websocket_server
    global websocket_thread

    if main_server is None:
        print("Start visualization with `serveVisualization {port}` first")
        return

    for uart, _ in uart_event_map:
        uart.CharReceived -= uart_event_map[uart]

    main_server.shutdown()
    main_server.server_close()
    websocket_server.close_server()
    server_thread.join()
    websocket_thread.join()
    main_server = None
    server_thread = None
    websocket_server = None
    websocket_thread = None
