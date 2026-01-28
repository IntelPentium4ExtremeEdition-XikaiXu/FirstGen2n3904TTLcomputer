module 3_bit_nor_gate{
	input logic a,
	input logic b,
	input logic c,
	output logic Y
};
	assign Y = ~(a | \
		     b | \
		     c 
		     );
endmodule