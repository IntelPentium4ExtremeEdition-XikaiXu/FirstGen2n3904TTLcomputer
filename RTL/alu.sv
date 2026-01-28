module ALU (
	//Data path
	input [7:0] logic logic_A_main, // Wired from Reg A
	input [7:0] logic logic_B_opa,  // Wired from Reg B

	//Operation controller, bit wise op
	input [7:0] logic op,           // Wired from Reg Instrcution Op
	
	//Data Output
	output [7:0] logic logic_Y,     // Wired to the BUS
	//flag Output  
	output logic N,                 // Negative, wire Reg A bit[7], signed number	
	output logic C,                 // Carry, ADDER C OUT/SHIFT LEFT OFT/
	output logic Z                  // Zero, ALU == 0/REG A == 0
);
// 256 instrcutions max:
// 0000 0000 -> HALT (NOR GATE CPU INTERRUPT)
// 0000 0001 -> Add (A = A + B)
// 0000 0010 -> Subtraction (A = A - B)
// 0000 0100 -> LDR (A <- MEM[ADDR])
// 0000 1000 -> STR (MEM[ADDR] <- A)
// 0001 0000 -> JMP (PC <- ADDR)
// 0010 0000 -> JZ (if Zero_flag == 1 then PC <- ADDR)
// 0100 0000 -> NOT (A <- ~A)
// 1000 0000 -> SHL (A <- A << 1)

always_comb begin
	if (op == 0'b00000001) begin
		adder_8bit adder(	
		.inputA(logic_A_main),
		.inputB(logic_B_opa),
		.cin(0),
		.outputY(logic_Y),
		.cout(overflow)
		);
	end 
	else if (op == 0'b00000010) begin
		wire [7:0] wire0;
		8bit_reverse reverse1(
			.val_in(logic_B_opa),
			.val_out(wire0)
		);
		adder_8bit adder(
			.inputA(logic_A_main),
			.inputB(wire0),
			.cin(1),
			.outputY(logic_Y),
			.cout(overflow)
			);
	end
	else if (op == 0'b0011) begin
		
	end
end 
endmodule