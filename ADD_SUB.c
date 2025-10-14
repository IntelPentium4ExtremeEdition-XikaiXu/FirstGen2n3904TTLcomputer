#include<stdio.h>
#include "logic_Gate.h"
 /* This is the c simulation of all of logic gate system for the integer calculation process of the TTL chip required to have
  * The cpu will major printed as this amount of stuff, this is just part of alu, each of the  C file represent the CPU structure
  */


void SinglebitAdder(bool &ans, bool &cout, bool &x, bool &y, bool %cin){
    bool wire0, wire1, wire2, wire3, wire4, wire5,wire6;
    nor_gate(wire0, x, y);
    nor_gate(wire1, wire0, x);
    nor_gate(wire2, wire0, y);
    nor_gate(wire3, wire1, wire2);
    nor_gate(wire4, wire3, cin);
    nor_gate(wire5, wire3, wire4);
    nor_gate(wire6, wire4, cin);
    nor_gate(ans, wire5, wire 6);
    nor_gate(cout, wire0, wire 4);
}
// the entire Adder is only 4 bit width, full adder structure
void FourBitAdderSubstractor(uint8_t &outputY,bool &cout,uint8_t &inputA,uint8_t &inputB,bool &cin){
    bool wire0, wire1, wire2;
    SinglebitAdder(outputY[0], wire0, inputA[0], inputB[0], cin);
    SinglebitAdder(outputY[1], wire1, inputA[1], inputB[1], wire0);
    SinglebitAdder(outputY[2], wire2, inputA[2], inputB[2], wire1);
    SinglebitAdder(outputY[3], cout, inputA[3], inputB[3], wire2);
}



