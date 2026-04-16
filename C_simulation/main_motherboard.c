#include<stdio.h>
#include<stdint.h>
#include<cpu.h>
#include<74HC161.h>
#include<RAM.h>
#include<ROM_64B.h>

/*
 *	This is the motherboard which used for connected all 
 *	of component from the cpu + program counter + ram + rom + output datapath
 *
 *	Timing diagram and validation is on the go and pending the validation, 
 *	first will built the logic board connection first for the testing 
 *	
 *	progress
 *
 *
 */ 
struct {
	bool clk;
	uint8_t wire; // 8 bit total, top 3 for op + 5 for enum
} wires_components;

int main(void){
    CPU cpu; //implimentating a real cpu
    
    //example program compute the sram fsram addr 0x10 and 0x11, str the value to ram 0x12

    cpu.sram[0x10] = 5;
    cpu.sram[0x11] = 3;

    //program store inside of the sram:
    uint8_t program[] = {  //PC
        LDARegA, 0x10,       //0x02 load sram[0x10] t  RegisterA
        ADD, 0x11,               //0x06 Increment Main register<F12>
        STA, 0x12,                 //0x08 Storage RegA content to sram 0x12
        HLT,0x00,
    };
    //burning process
    for (int i = 0; i < sizeof(program); i++){
        cpu.sram[i] = program[i];
    }
    //execution of the sys:
    while (!cpu.Halted){
        cpu_ticks(&cpu); //call the cpu we just createds
    }

    //output the result:
    printf("Result in memory[0x12]: %d\n", cpu.sram[0x12]);
}
