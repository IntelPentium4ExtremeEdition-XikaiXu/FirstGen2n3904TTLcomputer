#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>

// This file will contains the main componenent of the entire computer system

void  cpu_init_rst(CPU *cpu){
    cpu -> RegisterA = 0;
    cpu -> PC = 0;
    cpu -> zero_flag = false;
    for (int i = 0; i < 128; i++) {
        cpu->sram[i] = 0;
        cpu->sram[i] = 0;
    }
    cpu -> Halted = 0;
    //cpu st machine wit hthe correct stage schematic
    cpu -> st = 0;
    cpu -> opcode  = 0;
    cpu -> operand = 0;
    cpu -> clk = 0;
}

void cpu_ticks(CPU *cpu){
    if(cpu -> Halted) return;
    printf("current cpu clk is %u: PC = 0x%02X, State = %d, RegA = %d, zero_flag = %d\n", cpu->clk, cpu->PC,cpu->st, cpu->RegisterA,  cpu -> zero_flag);
    cpu -> clk++;
    switch(cpu ->st){
        case FETCH:
            cpu ->opcode = cpu -> sram[cpu -> PC];
            cpu -> operand = cpu -> sram[cpu -> PC + 1];
            printf("FETCH Operation: Opcode = 0x%02X, Operand = 0x%02X\n", cpu -> opcode, cpu -> operand);
            cpu ->st = EXECUTE;
            break;
        case EXECUTE:
            printf("EXECUTION Operation, the decoding happened under MUX selection\n");
            cpu_execute(cpu);
            cpu -> st = WRITE_BACK;
            break;
        case WRITE_BACK:
            printf("WRITE BACK Operation, the writing back process is happened under the MUX execution process");
            printf("PC updated to 0x%02X\n",cpu -> PC);
                if(cpu -> operand != 0 ||  cpu -> operand !=6) {
                cpu -> PC +=2 ;
            }
            cpu -> st = FETCH;
            break;
    }
}
int main(void){
    CPU cpu; //implimentating a real cpu
    cpu_init_rst(& cpu);
    //example program compute the sram fsram addr 0x10 and 0x11, str the value to ram 0x12

    cpu.sram[0x10] = 5;
    cpu.sram[0x11] = 3;

    //program store inside of the sram:
    uint8_t program[] = {  //PC
        LDARegA, 0x10,       //0x02 load sram[0x10] t  RegisterA
        ADD, 0x11,               //0x06 Increment Main register
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
