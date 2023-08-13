let ws = new WebSocket("ws://localhost:9001/")

function initBoard(self)
{
	SVGInject(self).then(() => {
		[0, 1, 2, 3, 4, 5, 6, 7].forEach((s) => {
			$("[id^=SW" + s + "BTN]")
				.mousedown(() => { ws.send("BTN|A" + s + "|Press") });
			$("[id^=SW" + s + "BTN]")
				.mouseup(() => { ws.send("BTN|A" + s + "|Release") });
			$("[id^=SW" + s + "LED]").attr("style", "fill: red");
		});
	});
}
document.initBoard = initBoard;

$(function() {
	var term = new Terminal({ fontSize: 10, rows: 20, cols: 80 });
	term.open(document.getElementById('terminal'));

	ws.onopen =
		function() {
		console.log("onopen")
	}

		ws.onmessage =
			function(e) {
		let split = e.data.split("|")
		let type = split[0] let name = split[1] let message = split[2]

			if (type == "LED")
		{
			console.log(e.data);
			if (message == "True") {
				$("[id^=" + name + "]").attr("style", "fill: green");
			} else {
				$("[id^=" + name + "]").attr("style", "fill: red");
			}
		}
		else if (type == "UART")
		{
			term.write(message);
		}
	}

			ws.onclose =
				function() {
		console.log("onclose")
	}

				ws.onerror = function(e) {
		console.log("onerror")
		console.log(e)
	}
})
