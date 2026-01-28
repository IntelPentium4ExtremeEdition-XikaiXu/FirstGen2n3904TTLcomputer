module ALU (
	//Data path
	input [7:0] logic logic_A_main, // Wired from Reg A
	input [7:0] logic logic_B_opa,  // Wired from BUS

	//Operation controller, bit wise op
	input [7:0] logic op,           // Wired from Reg Instrcution Op
	
	//Data Output
	output [7:0] logic logic_Y,     // Wired to the BUS or A Reg
	//flag Output  
	output logic N,                 // Negative, wire Reg A bit[7], signed number	to DFF
	output logic C,                 // Carry, ADDER C OUT/SHIFT LEFT OFT/ to DFF
	output logic Z,                 // Zero, ALU == 0/REG A == 0 to DFF
	
);
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

	//Extra component for ALU logic
	wire [7:0] wire0, wire1; // for SUB
	8bit_reverse reverse1; //reverser
	adder_8bit adder;
	not_gate N1;
	8_bit_nor_gate 8BNorG;
	// data path hard logic
	always_comb begin
		if (op == 0'b00000001) begin
			// ADD
			adder(	
				.inputA(logic_A_main),
				.inputB(logic_B_opa),
				.cin(0),
				.outputY(logic_Y),
				.cout(C)
			);
		end 
		else if (op == 0'b00000010) begin
			//SUB
			reverse1(
				.val_in(logic_B_opa),
				.val_out(wire0)
			);
			adder(
				.inputA(logic_A_main),
				.inputB(wire0),
				.cin(1),
				.outputY(logic_Y),
				.cout(C)
				);
		end
		else if (op == 0'b01000000) begin
			//NOT
			reverse1(
				.val_in(logic_A_main),
				.val_out(logic_Y)
			);
			C = 0;
		end
		else if (op == 0'b00001000) begin
			// SHL
			logic_Y[0] = 0;
			logic_Y[1] = logic_A_main[0];
			logic_Y[2] = logic_A_main[1];
			logic_Y[3] = logic_A_main[2];
			logic_Y[4] = logic_A_main[3];
			logic_Y[5] = logic_A_main[4];
			logic_Y[6] = logic_A_main[5];
			logic_Y[7] = logic_A_main[6];
			C = logic_A_main[7];
		end
		else begin
			// flag path hard logic
			logic_Y = 0;
			C = 0;
		end
	end 
	//flag Negative
	assign N = logic_A_main[7];
	//flag Carry
	8BNorG(
		.val_A(logic_A_main),
		.Y(wire1)
	);
	N1(
		.a(wire1),
		.y(Z)
	);
endmodule