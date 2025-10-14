#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
// #include "logic_Gate.h"
// #include "instructionsets.h"

typedef enum{
    MOV, //000
    LDARegA,   //001
    STA,    //010
    ADD,    //011
    SUB,    //100
    JMP,    //101
    LDARegB, //110
    HLT,    //111
}Instructions;

typedef enum {
    FETCH,
    DECODE,
    EXECUTE,
    WRITE_BACK
} CPUPipeline;

typedef struct {
    uint8_t RegisterA;
    uint8_t RegisterB;
    uint8_t PC;
    bool zero_flag;
    uint8_t sram[128]; //128 Byte of memory
    uint8_t rom[128]; //128 Byte of ROM, for data Storagement
    bool Halted; //cpu stopped
    uint32_t clk;
    CPUPipeline st;
    uint8_t opcode;
    uint8_t operand;
} CPU;

void  cpu_init_rst(CPU *cpu){
    cpu -> RegisterA = 0;
    cpu -> RegisterB = 0;
    cpu -> PC = 0;
    cpu -> zero_flag = false;
    for (int i = 0; i < 128; i++) {cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC += 2;
            break;
        cpu->sram[i] = 0;
        cpu->rom[i] = 0;
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
        case MOV:
            cpu -> RegisterA = operand;
            cpu ->zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC += 2;
            break;
        case LDARegA:
            cpu -> RegisterA = cpu ->rom[operand & 0x7F];
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu ->PC +=2;
            break;
        case STA:
            cpu ->sram[operand & 0x7F] = cpu ->RegisterA;
            cpu ->PC += 2;
            break;
        case ADD:
            cpu -> RegisterA = (cpu->RegisterA + cpu -> RegisterB) &0xFF; // 8 bit overflow?
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC += 2;
            break;
        case SUB:
            cpu -> RegisterB = (cpu -> RegisterA - cpu -> RegisterB) & 0xFF;
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            cpu -> PC += 2;
            break;
        case JMP:
            cpu -> PC = operand;
            break;
        case LDARegB:
            cpu -> RegisterB = cpu -> rom[operand & 0x7F];
            cpu -> PC += 2;
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
    //example program compute the ROM from addr 0x10 and 0x11, str the value to ram 0x12

    cpu.rom[0x10] = 5;
    cpu.rom[0x11] = 3;
    cpu.RegisterB = cpu.rom[0x11]; //since the cpu does not have that feature of load the reg for now, pre load the data to verified the feature;

    //program store inside of the ROM:
    uint8_t program[] = {    //PC
        MOV, 0x00,                 //0x00 clean the reg A
        LDARegA, 0x10,                  //0x02 load rom[0x10] t  RegisterA
        ADD, 0x00,                 //0x04 RegA = RegA+RegB
        STA, 0x12,       //0x06 Storage RegA content to sram 0x12
        HLT,                             //0x08 Stop the operation
    };
    for (int i = 0; i < sizeof(program); i++){
        cpu.rom[i] = program[i];
    }

    //execution of the sys:
    while (!cpu.Halted){
        uint8_t opcode = cpu.rom[cpu.PC];
        uint8_t operand = cpu.rom[cpu.PC + 1];
        printf("PC: 0x%02X, Opcode: 0x%02X, operand: 0x%02X, RegA: %d, RegB: %d, ZF: %d\n", cpu.PC, opcode, operand, cpu.RegisterA, cpu.RegisterB, cpu.zero_flag);
        cpu_execute(&cpu, opcode, operand);
    }

    //output the result:
    printf("Result in memory[0x12]: %d\n", cpu.sram[0x12]);
}
