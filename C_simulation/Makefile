CC 	= gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
TARGET 	= sim
SRC 	= main_motherboard.c
HEADERS = CPU.h ROM_64B.h SRAM.h 74HC161.h 

.PHONY: all run clean 

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./(TARGET)

clean: 
	rm -f $(TARGET) $(TARGET) 
