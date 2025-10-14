#include<stdio.h>
typedef enum {
    FETCH,
    DECODE,
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
    LDARegB, //0110
    HLT,    //0111
}Instructions;

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
