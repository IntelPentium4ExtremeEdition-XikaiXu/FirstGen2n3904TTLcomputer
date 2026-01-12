`timescale 1ns/1ps

module tb_nor_gate;
	logic a;
	logic b;
	logic y;
	nor_gate dut(
		.a(a),
		.b(b),
		.y(y)
	);

	initial begin
		$dumpfile("nor_gate.vcd");
		$dumpvars(0, tb_nor_gate);

		a = 0; b = 0; #10;
		a = 1; b = 1; #10;
		a = 1; b = 0; #10;
		a = 0; b = 1; #10;

		$finish;
	end
endmodule

