"""
2N3904 Quick quick testing portaion -Extend Triend
128Byte of "ROM", 10Byte RAM, but due to 5bit data so its 16Bites
CPU: 3 bit op + 5 bit oprand
NULL ROM, PC 2* 74HC161 Program counter fully functional chipset
Jack Xu 2026
"""

import sys

#74HC161 Program counter LOL
class HC74_161:
    def __init__(self):
        self.Q = 0
        self.clk = False
        self.CLR_n = True
        self.LOAD_n = True
        self.ENP = False
        self.ENT 

