


module 8bit_reverse (
	//data in
	input logic [7:0] val_in,

	//data out
	output logc [7:0] val_out
	);
	not_gate g7(
		.a(val_in[7]),
		.y(val_out[7])
		);
	not_gate g6(
		.a(val_in[6]),
		.y(val_out[6])
		);
	not_gate g5(
		.a(val_in[5]),
		.y(val_out[5])
		);
	not_gate g4(
		.a(val_in[4]),
		.y(val_out[4])
		);
	not_gate g3(
		.a(val_in[3]),
		.y(val_out[3])
		);
	not_gate g2(
		.a(val_in[2]),
		.y(val_out[2])
		);
	not_gate g1(
		.a(val_in[1]),
		.y(val_out[1])
		);
	not_gate g0(
		.a(val_in[0]),
		.y(val_out[0])
		);
endmodule
