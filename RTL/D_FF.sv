module d_ff(
	//clk syncronizer
	input logic clk,
	input logic D,
	
	output logic Q,
	output logic QN
);

//required nor the module
nor_gate NOR0,NOR1,NOR2,NOR3,NOR4;
3_bit_nor_gate NOR5;
logic wire0, wire1, R, S;
	NOR0(
		.a(wire0),
		.b(R),
		.y(wire1)
	)
	NOR1(
		.a(wire1),
		.b(clk),
		.y(R)
	)
	NOR2(
		.a(R),
		.b(QN),
		.y(Q)
	)
	NOR3(
		.a(Q),
		.b(S),
		.y(QN)
	)
	NOR4(
		.a(S),
		.b(D),
		.y(wire0)
	)
	NOR5(
		.a(R),
		.b(clk),
		.c(wire0),
		.y(S)
	)
endmodule