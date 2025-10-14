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
