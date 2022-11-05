`default_nettype	none
module	plc(
	clk,
	l_level,
	h_level,
	stop,
	mixer,
	pump_1,
	pump_2,
	valve
);
	parameter	[15:0]	MIX_DELAY = 7000;
	input	wire	clk;
	input	wire	l_level;
	input	wire	h_level;
	input	wire	stop;
	output	reg	mixer = 0;
	output	reg	pump_1 = 0;
	output	reg	pump_2 = 0;
	output	reg	valve = 0;

	reg	[15:0]	counter = 0;

	always @(posedge clk) begin
		// if we are in the process of mixing
		if (counter != 0) begin
			counter = counter - 1'b1;
			mixer = 1;
			if(counter == 0) begin
				valve = 1;
				mixer = 0;
			end
		else
			mixer = 0;
		end
		// If we are draining the tank and lower level is reached
		if(h_level == 0 && l_level == 0 && valve) begin
			// close the valve
			valve = 0;
			// start pumps
			pump_1 = 1;
			pump_2 = 1;
		end
		// If we are filling the tank and upper level is reached
		if(l_level && h_level && pump_1 && pump_2) begin
			// turn off the pumps
			pump_1 = 0;
			pump_2 = 0;
			// turn on the mixer
			mixer = 1;
			counter = MIX_DELAY-1'b1;
		end
		// if we are half way full and valve is off we start refill
		if(valve == 0 && h_level == 0 && l_level == 1) begin
			pump_1 = 1;
			pump_2 = 1;
		end
		// if tank is empty then start the fill
		if(valve == 0 && h_level == 0 && l_level == 0) begin
			pump_1 = 1;
			pump_2 = 1;
		end
	end

	always @(*) begin
		if ((stop)) begin
			counter = 0;
			mixer = 0;
			pump_1 = 0;
			pump_2 = 0;
			valve = 0;
		end
	end

`ifdef	FORMAL
	always @(*) begin
		// counter non-zero is only valid when tank is full
		if(counter != 0) begin
			assert(h_level);
			assert(l_level);
			// valve must be closed when we start mixing
			assert(valve == 0);
		end
		// mixer should never be active when valve is open
		if(mixer) begin
			assert(valve == 0);
		end
		// unless the tank is full we should never be mixing
		if(h_level == 0) begin
			assert(mixer == 0);
		end
		// counter must always have value less than max delay
		assert(counter < MIX_DELAY);
		// pumps must always work at the same time
		assert(pump_1 == pump_2);
		// if stop is active all valves and pumps must be off!
		if(stop) begin
			assert(mixer == 0);
			assert(pump_1 == 0);
			assert(pump_2 == 0);
			assert(valve == 0);
		end
	end
`endif
endmodule
