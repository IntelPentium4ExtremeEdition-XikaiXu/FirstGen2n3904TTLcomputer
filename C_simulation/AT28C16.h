#ifndef AT28C16_H
#define AT28C16_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
//Downgrade to AT28C16
//2K * 8bit
//A0 to A10
//D0 to D7
//controll signal
//CE, OE, WE 

#define AT28C16_SIZE 2048
#define AT28C16_ADDR_MASK 0x7FF

typedef struct{
	uint8_t rom[AT28C16_SIZE]; //storage content
	bool CE_n;	//low is effective
	bool OE_n;	//low is effective 
	bool WE_n;	//low is effective 
	
	//address & data bus
	uint16_t address; //current address
	uint8_t data_bus; //data buss taking the leads of input and output

	//simulation for the "writing cycle"
	bool busy; //busy state 
	uint32_t write_cycle_counter; //write cycle counter consumption
	
	//used for data cycling access
	uint16_t last_addr; //last time address in
	uint8_t last_data; //last time data in 

} AT28C16;

//init of the entire chipset 
void at28c16_init(AT28C16 *chip);

//rest of the chip
void at28c16_rst(AT28C16 *chip);

//setting the address of the chipset
void at28c16_set_address(AT28C16 *chip , uint16_t address);

//setting the controller logic of the chip
void at28c16_set_flags(AT28C16 *chip, bool CE, bool OE, bool WE);

//reading hex val from sram
uint8_t at28c16_read(AT28C16 *chip);

// loading the hex file, used for after programming the chip
int at28c16_burning(AT28C16 *chip, const char *filename);

// function defined region
void at28c16_init(AT28C16 *chip){
	//rest the value to the default, this is the normal init for all val to 0xff
	memset(chip -> rom, 0xFF,AT28C16_SIZE);
	//triggered the reset func
	at28c16_reset(chip);
}

void at28c16_rst(AT28C16 *chip){
	chip -> CE_n = true;
	chip -> OE_n = true;
	chip -> WE_n = true;
	chip -> address = 0;
	chip -> data_bus = 0xFF;
	chip -> busy = false;
	chip -> write_cycle_counter = 0;
	//rest the data call var
	chip -> last_addr = 0;
	chip -> last_data = 0xFF;
}

void at28c16_set_address(AT28C16 *chip, uint16_t address_target){
	//only take the low 11 bit for the data addressing line
	chip -> address = address_target & AT28C16_ADDR_MASK;
}

void at28c16_set_flags(AT28C16 *chip, bool CE, bool OE, bool WE){
	//record the prev state of the WE_n statement
	bool WE_n_prev = chip -> WE_n;

	//set the signal, when value is true, set the val to low 	
	chip -> CE_n = !CE;
	chip -> OE_n = !OE;
	chip -> WE_n = !WE;
	
	//detection on the posedge: WE_n from 0 to 1, as Enable to disable
	if(!WE_n_prev && chip -> WE_n){
		if(!chip -> CE_n && chip -> OE_n && !chip->busy){
			//checking the input val process statement:
			chip -> rom[chip -> address] = chip -> data_bus;
			//set the busy cycle: (guess 10ms, 100000 cycle clk, each clk is 1us)
			chip -> busy = true;
	       		chip -> write_cycle_counter = 10000;
		
			//record data used for reading
			chip -> last_addr = chip -> address;
			chip -> last_data = chip -> data_bus;	
		}
	}	
}

uint8_t at28c16_read(AT28C16 *chip){
	//CE = 0, and OE = 0, we will have the data, otherwise will not
	if (!chip -> OE_n && !chip -> CE_n){
		chip -> data_bus = chip -> rom[chip -> address];
	}
	else {
	//otherwise the output will just a mess:
		chip -> data_bus = 0xFF;
	}
	return chip -> data_bus;
}	

int at28c16_burning(AT28C16 *chip, const char *filename) {
	FILE *f = fopen(filename, "r");
	if(!f) return -1;
	
	memset(chip -> rom, 0xFF, AT28C16_SIZE);
	char lane[64];
	while(fgets(line,sizeof(line),f)){
		unsigned int addr, val;
		//matched the format rom XXXX: YY
		if(sscanf(line, "row%x: %x", &addr, &val) == 2){
			if(addr <2048) {
				rom[addr] = (uint8_t)val;
			}
		}
	}
	fclose(f);
	return 0;
}	
#endif 
