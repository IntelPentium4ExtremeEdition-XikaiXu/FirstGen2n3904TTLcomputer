#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
1602 LCD Simulator (receives bytes via named pipe)
Usage: python3 lcd1602_gui.py
Protocol: Each byte received: bit7=RS (1=data, 0=command), bit6-0 = data
"""

import tkinter as tk
import os
import sys
import threading
import time

LCD_COLS = 16
LCD_ROWS = 2

# HD44780 commands
LCD_CLEAR   = 0x01
LCD_HOME    = 0x02
LCD_DISPLAY = 0x08
LCD_SHIFT   = 0x10
LCD_FUNC    = 0x20
LCD_CGRAM   = 0x40
LCD_DDRAM   = 0x80

class LCD1602:
    def __init__(self, root):
        self.root = root
        self.root.title("1602 LCD")
        self.root.resizable(False, False)
        self.ddram = [[' ']*LCD_COLS for _ in range(LCD_ROWS)]
        self.cgram = [[0]*5 for _ in range(8)]
        self.col = 0
        self.row = 0
        self.display_on = True
        self.cursor_on = False
        self.blink_on = False

        self.cw, self.ch = 16, 24
        self.canvas = tk.Canvas(root, width=LCD_COLS*self.cw+20,
                                height=LCD_ROWS*self.ch+20, bg='#222')
        self.canvas.pack(padx=5, pady=5)
        self.draw()

    def draw(self):
        self.canvas.delete('all')
        for r in range(LCD_ROWS):
            for c in range(LCD_COLS):
                x1 = c*self.cw + 10
                y1 = r*self.ch + 10
                x2 = x1 + self.cw
                y2 = y1 + self.ch
                bg = '#9bbc0f' if self.display_on else '#555'
                fg = '#1a1a1a'
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=bg, outline='#0f0f0f')
                self.canvas.create_text(x1+self.cw//2, y1+self.ch//2,
                                        text=self.ddram[r][c], fill=fg,
                                        font=('Courier', 12, 'bold'))
        self.root.after(500, self.draw)

    def write(self, rs, data):
        if rs == 0:   # command
            if data == LCD_CLEAR:
                for r in range(LCD_ROWS):
                    for c in range(LCD_COLS):
                        self.ddram[r][c] = ' '
                self.col = self.row = 0
            elif data == LCD_HOME:
                self.col = self.row = 0
            elif data & LCD_DISPLAY:
                self.display_on = bool(data & 0x04)
                self.cursor_on = bool(data & 0x02)
                self.blink_on = bool(data & 0x01)
            elif data & LCD_DDRAM:
                addr = data & 0x7F
                if addr < 0x40:
                    self.row = 0
                    self.col = addr
                else:
                    self.row = 1
                    self.col = addr - 0x40
                if self.col >= LCD_COLS:
                    self.col = LCD_COLS - 1
        else:           # data (character)
            self.ddram[self.row][self.col] = chr(data)
            self.col += 1
            if self.col >= LCD_COLS:
                self.col = 0
                self.row = 1 if self.row == 0 else 0

def fifo_listener(lcd, fifo_path):
    while True:
        try:
            with open(fifo_path, 'rb') as f:
                while True:
                    b = f.read(1)
                    if not b: break
                    byte = b[0]
                    rs = 1 if (byte & 0x80) else 0
                    data = byte & 0x7F
                    lcd.write(rs, data)
        except (BrokenPipeError, FileNotFoundError):
            time.sleep(0.1)

def main():
    fifo = '/tmp/cpu_lcd_fifo'
    if not os.path.exists(fifo):
        os.mkfifo(fifo)
    root = tk.Tk()
    lcd = LCD1602(root)
    threading.Thread(target=fifo_listener, args=(lcd, fifo), daemon=True).start()
    root.mainloop()

if __name__ == '__main__':
    main()