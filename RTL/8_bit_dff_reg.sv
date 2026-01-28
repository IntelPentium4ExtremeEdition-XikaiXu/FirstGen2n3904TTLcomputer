module 8_bit_reg(
	input clk,
	input logic [7:0] D,
	
	output logic [7:0] Q,
	output logic [7L0] QN
);
d_ff DFF0,DFF1,DFF2,DFF3,DFF4,DFF5,DFF6,DFF7;
	DFF0(
		.D(D[0]),
		.clk(clk),
		.Q(Q[0]),
		.QN(QN[0])
	);
	DFF1(
		.D(D[1]),
		.clk(clk),
		.Q(Q[1]),
		.QN(QN[1])
	);
	DFF2(
		.D(D[2]),
		.clk(clk),
		.Q(Q[2]),
		.QN(QN[2])
	);
	DFF3(
		.D(D[3]),
		.clk(clk),
		.Q(Q[3]),
		.QN(QN[3])
	);
	DFF4(
		.D(D[4]),
		.clk(clk),
		.Q(Q[4]),
		.QN(QN[4])
	);
	DFF5(
		.D(D[5]),
		.clk(clk),
		.Q(Q[5]),
		.QN(QN[5])
	);
	DFF6(
		.D(D[6]),
		.clk(clk),
		.Q(Q[6]),
		.QN(QN[6])
	);
	DFF7(
		.D(D[7]),
		.clk(clk),
		.Q(Q[7]),
		.QN(QN[7])
	);
endmodule
