#include<stdio.h>
typedef enum {
    FETCH,
    EXECUTE,
    WRITE_BACK
} CPUPipeline;

typedef enum{
    JZ, //0000
    LDARegA,   //0001
    STA,    //0010
    ADD,    //0011
    SUB,    //0100
    JMP,    //0101
    HLT,    //0111
}Instructions;

typedef struct {        //Addr
    uint8_t RegisterA; // Individual Register DFF
    uint8_t PC;             //  individual register, might be 12 bit self incrementing register
    bool zero_flag;      // individual DFF
    uint8_t sram[2048]; //2K Byte of memory// currently only support 256 Byte  [6116 Chipset]
    bool Halted;           //cpu stopped //individual

    CPUPipeline st;     // 4 reg sequence, 4 D FF with mux
    uint8_t opcode;    //no reg required, hard circuit -> fake, no register required
    uint8_t operand;  //no reg required, hard circuit -> fake, no register required
    uint32_t clk;           //clk generating --> fake, no register needeed
} CPU;

void cpu_execute(CPU *cpu);

void cpu_execute(CPU *cpu){
    switch (cpu -> opcode){
        case JZ:
            if (cpu -> zero_flag){
                cpu -> PC = cpu -> operand;
            }
            break;
        case LDARegA:
            cpu -> RegisterA = cpu ->sram[cpu -> operand];
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            break;
        case STA:
            cpu ->sram[cpu -> operand] = cpu ->RegisterA;
            break;
        case ADD:
            cpu -> RegisterA = (cpu->RegisterA + cpu -> sram[cpu -> operand]); // 8 bit overflow?
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            break;
        case SUB:
            cpu -> RegisterA = (cpu -> RegisterA - cpu -> sram[cpu -> operand]);
            cpu -> zero_flag = (cpu -> RegisterA == 0);
            break;
        case JMP:
            cpu -> PC = cpu -> operand;
            break;
        case HLT:
            cpu -> Halted = 1;
            printf("lol, halt is there\n");
            break;
        default:
            printf("unknown instrcution: 0x%02X\n", cpu -> opcode);
            cpu->Halted = true;
            break;
    }
}

