#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
// #include "logic_Gate.h"
#include "Cpu_sub_st.h"


typedef struct {        //Addr
    uint8_t RegisterA; // 0x0
    uint8_t RegisterB; // 0x1
    uint8_t PC;             //  0x2
    bool zero_flag;      // individual
    uint8_t sram[2048]; //2K Byte of memory// currently only support 256 Byte
    bool Halted; //cpu stopped //individual
    uint32_t clk;      //clk generating
    CPUPipeline st; // 0x7
    uint8_t opcode; //0x8
    uint8_t operand; //0x9
} CPU;

void  cpu_init_rst(CPU *cpu){
    cpu -> RegisterA = 0;
    cpu -> RegisterB = 0;
    cpu -> PC = 0;
    cpu -> zero_flag = false;
    for (int i = 0; i < 128; i++) {
        cpu->sram[i] = 0;
        cpu->sram[i] = 0;
    }
    cpu -> Halted = false;

    cpu -> st = FETCH;
    cpu -> opcode  = 0;
    cpu -> operand = 0;
    cpu -> clk = 0;
}

void cpu_execute(CPU *cpu, uint8_t opcode, uint8_t operand){
    if (true == cpu->Halted){
        return;
    }
    cpu -> clk++;
    //printf("current clk cycle: %d, PC: 0x%02X, State = %d, RegA = %d, RegB = %d, ZF = %d\n" )
    Instructions mop = (Instructions)opcode;
    switch (mop){
        case JZ:
            if (cpu -> zero_flag){
                cpu -> PC = operand;
            }
            else{
                cpu -> PC+=2;
            }
            break;
        case LDARegA:
            cpu -> RegisterA = cpu ->sram[operand & 0x7F];
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC+=2;
            break;
        case STA:
            cpu ->sram[operand & 0x7F] = cpu ->RegisterA;
            cpu -> PC+=2;
            break;
        case ADD:
            cpu -> RegisterA = (cpu->RegisterA + cpu -> RegisterB) &0xFF; // 8 bit overflow?
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC+=2;
            break;
        case SUB:
            cpu -> RegisterB = (cpu -> RegisterA - cpu -> RegisterB) & 0xFF;
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC+=2;
            break;
        case JMP:
            cpu -> PC = operand;
            break;
        case LDARegB:
            cpu -> RegisterB = cpu -> sram[operand & 0x7F];
            cpu -> PC+=2;
            break;
        case HLT:
            cpu -> Halted = true;
            break;
        default:
            printf("unknown instrcution: 0x%02X\n", opcode);
            cpu->Halted = true;
            break;
    }
}
int main(void){
    CPU cpu;
    cpu_init_rst(& cpu);
    //example program compute the sram fsram addr 0x10 and 0x11, str the value to ram 0x12

    cpu.sram[0x10] = 5;
    cpu.sram[0x11] = 3;
    cpu.RegisterB = cpu.sram[0x11]; //since the cpu does not have that feature of load the reg for now, pre load the data to verified the feature;

    //program store inside of the sram:
    uint8_t program[] = {    //PC
        LDARegA, 0x00,        //0x00 load clean reg fsram sram0
        LDARegA, 0x10,        //0x02 load sram[0x10] t  RegisterA
        LDARegB, 0x11,
        ADD, 0x00,                //0x0 RegA = RegA+RegB
        STA, 0x12,                  //0x06 Storage RegA content to sram 0x12
        LDARegA, 0x12,
        JMP, 0x06,
    };
    for (int i = 0; i < sizeof(program); i++){
        cpu.sram[i] = program[i];
    }
    //execution of the sys:
    while (!cpu.Halted){
        uint8_t opcode = cpu.sram[cpu.PC];
        uint8_t operand = cpu.sram[cpu.PC + 1];
        printf("PC: 0x%02X, Opcode: 0x%02X, operand: 0x%02X, RegA: %d, RegB: %d, ZF: %d\n", cpu.PC, opcode, operand, cpu.RegisterA, cpu.RegisterB, cpu.zero_flag);
        cpu_execute(&cpu, opcode, operand);
    }

    //output the result:
    printf("Result in memory[0x12]: %d\n", cpu.sram[0x12]);
}
