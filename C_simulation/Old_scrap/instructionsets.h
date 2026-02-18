#include<stdio.h>

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
