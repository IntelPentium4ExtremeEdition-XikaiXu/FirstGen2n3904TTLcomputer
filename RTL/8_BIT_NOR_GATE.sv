module 8_bit_nor_gate{
	input logic [7:0] val_A,
	output logic Y
};
	assign Y = ~(val_A[0] | \
			val_A[1] | \
			val_A[2] | \
			val_A[3] | \
			val_A[4] | \
			val_A[5] | \
			val_A[6] | \
			val_A[7] 
			);
endmodule