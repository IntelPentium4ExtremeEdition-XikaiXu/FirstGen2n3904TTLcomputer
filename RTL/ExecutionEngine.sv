module EU{
	input logic clk,
	input logic rst, //used for init all of the 3 register inside of the EU 

	//Thinking the extra componenet that exposed to the outside world
	wire logic [7:0] bus,
	wire logic [13:0] address

	output logic Flag_N,
	output logic Flag_C,
	output logic Flag_Z,
};
//brief explantion for the 
	// 256 instrcutions max:
	// 0000 0000 -> HALT (NOR GATE CPU INTERRUPT)
	// 0000 0001 -> ADD  (A(ACCUMLATE_NEO) = A(ACCUMLATE_OLD) + B)
	// 0000 0010 -> SUB  (A(ACCUMLATE_NEO) = A(ACCUMLATE_OLD) - B)
	// 0000 0100 -> LDR  (A <- MEM[ADDR])
	// 0000 1000 -> STR  (MEM[ADDR] <- A)
	// 0001 0000 -> JMP  (PC <- ADDR)
	// 0010 0000 -> JZ   (if Zero_flag == 1 then PC <- ADDR)
	// 0100 0000 -> NOT  (A <- ~A)
	// 1000 0000 -> SHL  (A <- A << 1)a
//building block
//ALU exponent
ALU alu1;
wire [7:0] wire_8_0, wire_8_1, wire_8_2, wire_op;
//3 flag register
logic Reg_N, Reg_C, Reg_Z;

logic [7:0] Reg_A, IncrementReg, operation_reg;
 
alu1{
	.logic logic_A_main(), // Wired from Reg A
	.logic_B_opa(),  // Wired from BUS
	//Operation controller, bit wise op
	.op(),           // Wired from Reg Instrcution Op
	//Data Output
	.logic_Y(),     // Wired to the BUS or A Reg
	//flag Output  
	.N(Reg_N),                 // Negative, wire Reg A bit[7], signed number	to DFF
	.C(Reg_C),                 // Carry, ADDER C OUT/SHIFT LEFT OFT/ to DFF
	.Z(Reg_Z)                 // Zero, ALU == 0/REG A == 0 to DFF
};

always_ff@(posedge clk or negedge rst) begin
	if(!rst) begin
		
	end
	else begin
		
	end
end

endmodule
