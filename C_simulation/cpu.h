#include<stdio.h>
/* This is the homebrew CPU
 * after mass amount of thinking and make tons of disicion, sel ins + data arch
 * 3 bit ins + 5 bit data, total 8 bit length
 * PC is 12 bit total, hope able to video output with 1602 LCD 
 * 3 cycle file the addr, total 8 instructions , 2N3902 NOR GATE BASED
 * Jack Xu, 2025-06-01 to pending 
 */
#ifndef CPU_H
#define CPU_H

// total instructions
typedef enum{
    NOP,      	    //000 - null processing, do nothing, used for filling, aligned, timing lentancy process - null flag - null op
    LDA_imm5        //001 - init ACC register, setting default value - zero flag(if imm5 == 0, z = 1) - acc <- imm5
    ADD_imm5,       //010 - acculmate, PC counter increment - Z/C - acc <- acc + imm5
    SUB_imm5,       //011 - Subtraction, comparision - Z/C - acc <- acc - imm5
    AND_imm5,       //100 - bit masking, testing certain bit, clear bit - Z/C - acc <- acc & imm5
    SHR,            //101 - reg left shifing, used for driving the lcd/dividing  - Z/C - acc <- acc >> 1
    JZ_imm5,        //110 - statement jump, if statement ACC = 0 detecting - null flag - if Z =1, PC <- PC + sign_extend(imm5) -might 3 cycle
    OUT_imm5        //111 - ACC to the mainbus, sending the data through this inst - null flag - mainbus <- acc 
} cpu_instructions;

// total registers 
typedef struct {        
    uint8_t AcculmateReg; //only use 5 bit of it
    bool Z;               //zero flag register
    bool C;               //carry flag register
} cpu_reg_struct;

//total execution paths 
void alu(uint8_t *input_A, uint8_t *input_B, uint8_t *output, bool C_in){
	if (C_in == 1){
		output = input_A - ~input_B;
	}
	else{
		output = input_A + input_B;
	}
}

void eu(){
}

void cpu{
}
#endif
