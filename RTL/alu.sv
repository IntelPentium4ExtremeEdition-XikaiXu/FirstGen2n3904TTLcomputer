


module ALU (
	//Data path
	input [7:0] logic logic_A_main,
	input [7:0] logic logic_B_opa,
	input [3:0] logic operation_code_alu,
	//Dataput Put
	output [7:0] logic logic_Y,
	output logic overflow,
	output logic sign,
	output logic carry
);

assign 
