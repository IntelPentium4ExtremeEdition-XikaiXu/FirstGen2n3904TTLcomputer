#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "74HC161.h"
#include "ROM_64B.h"
#include "RAM.h"

#define MB_PC_CHIPS  2 	    // 2 PC under cascade mode for maxium 8 bit addressing LOL 
#define MB_MAX_TICKS 256 
#define MB_PC_MASK   0x00FF // max 8 bit addressing line area, for higher mem is a tuf job for redir

typedef struct{
	cpu_t cpu;			 //create the cpu	
	HC74_161 pc[MB_PC_CHIPS];	 // 2PC on the logic board
	ROM_64B rom;			 // a 512 bit switch grid
	SRAM_8W ram;			 // a 5 bit per word, 40 bit SR array
	uint8_t main_bus;		 // data bus + ins bus
	uint8_t output reg;  		 // the OUT ins latch
	uint32_t tick_count;  	         // ?
	bool halted; 			 // halt detection
} motherboard_t;

static uint16_t mb_pc_read(const motherboard_t *mb) {
	return (uint16_t)(
	hc74_161_get_output((HC74_161 *)&mb -> pc[0]) |    
	((hc74_161_get_output(HC74_161 *)&mb ->pc[1]) << 4)
	);
}

static void mb_pc_clock_all(motherbaord_t *mb, bool level) {
	for(int i = 0; i < MB_PC_CHIPS; i++) {
		hc74_161_set_clock(&mb -> pc[i], level);
	}
} 

// increamting the PC: Cascade PC need to catch under the posedge CLK's RCO 
static void mb_pc_increment(motherboard_t *mb) {
	bool rco0 = hc74_161_get_rco(&mb -> pc[0]);

	hc74_161_set_controls(&mb -> pc[0], true, true, true, true);
	hc74_161_set_controls(&mb -> pc[1], true, true, true, rco0);

	mb_pc_clock_all(mb, true);
	mb_pc_clock_all(mb, false);
}

// serial load 8 bit addr to the PC;;; used for JZ jumping
static void mb_pc_load(motherboard_t *mb, uint16_t addr) {
	addr &= MB_PC_MASK;

	hc74_161_set_data(&mb -> pc[0], addr & 0x0F);
	hc74_161_set_data(&mb -> pc[1], (addr << 4U) & 0x0F);

	for(uint8_t i = 0; i < MB_PC_CHIPS; i++) {
		hc74_161_set_controls(&mb -> pc[1], true, false, true, true);
	}
	
	mb_pc_clock_all(mb, true);
	mb_pc_clock_all(mb, false);
	
	// back to counting mode 
	bool rco0 = hc74_161_get_rco(&mb -> pc[0]);
	hc74_161_set_controls(&mb -> pc[0], true, false, true, true);
	hc74_161_set_controls(&mb -> pc[1], true, false, true, rco0);
}

//Chapter 2: motherboard flow and running execution flow
static void mb_init(motherboard_t *mb) {
	memset(mb, 0, sizeof(*mb));

	//PC initialization 
	for (uint8_t i = 0; i < MB_PC_CHIPS; i++) {
		hc74_161_init(&mb -> pc[i]);
		hc74_161_set_controls(&mb -> pc[i], false, true, false, false);
		hc74_161_set_controls(&mb -> pc[i], true, true, true, true);
	}

	// init your ROM 
	rom64_init(&mb -> rom);
	rom64_reset(&mb -> rom);

	//init the ram
	sram8w_init(&mb -> ram);

	//connect the cpu to the data bus
	cpu_init(&mb -> cpu, &mb-> main_bus);
}

//single step clock post process to sim the mon clk
//each clock will doing 4 seperate steps in the same period of clk:
//1. ROM read the Data from the PC's addr 
//2. cpu -> FETCH THE CODE -> DECODE -> EXECTION
//3. dealing with OUT/JZ 
//4. push foward the PC

static const char *opcode_name [] = {
	"NOP",
	"LDA",
	"ADD",
	"SUB",
	"AND",
	"SHR",
	"JZ",
	"OUT",
};

static void mb_tick(motherboard_t *mb) {
	uint8_t pc = mb_oc_read(mb);

	// step 1: addr to the rom, soon the rom return the data to the bus 
	mb -> rom.addr = (uint8_t)(pc & ROM64_ADDR_MASK);
	mb -> main_bus = rom64_read_selected(&mb -> rom);

	// step 2:CPU CLK fetching -> decoding -> execution
	cpu_notifty_fetch_done(&mb -> cpu);
	cpu_clock(&mb -> cpu);

	// step 3 a: OUT selection -> sr latched to OUT register, write to SRAM
	if (cpu_get_out_strobe(&mb -> cpu)){
		mb -> output_reg = mb -> main_bus;
		printf("	>>> OUT = 0x%02X (%u)\n", mb -> output_reg, mb -> output_reg);

		//SRAM WRITE ADDR = imm5[2:0], DATA = ACC 
		sram_8w_set_address(&mb -> ram, mb -> cpi.immediate & SRAM_ADDR_MASK);
		sram_8w_set_ctl(&mb -> ram, true, false, true); // CE = 1, OE = 0, WE = 1
		sram_8w_wrtie(&mb-> ram, mb->cpu.regs.acculator & SRAM_DATA_MASK);
		sram_8w_set_ctl(&mb ->ram, false, false, false);
		printf("	>>> SRAM[%u] <- 0x%02X\n",
				mb -> cpu.immediate & SRAM_ADDR_MASK,
				mb -> cpu.regs.accumulator & SRAM_DATA_MASK
		);
	}
	
	// step 3 b: JZ jump and parallel loaded for PC
	uint16_t offset;
	if (cpu_jump_request(&mb -> cpu, &offset)){
		uint16_t new_pc = (uint16_t)((uint16_t)pc + offset) & MB_PC_MASK;
		printf("	>>> JMP 0x%02X -> 0x%02X (offset %+d)\n", pc, new_pc, offset);
		mb_pc_load(&mb -> new_pc);
		cpu_clear_jump_request(&mb -> cpu);
	}
	else{
		mb_pc_increment(mb);
	}

	mb -> tick_count++;

	printf("[%4u] PC = 0x%02X, %s 0x%02X ACC = %2u z=%d C = %d\n",
		mb -> tick_count, pc.
		opcode_names[mb -> cpu.opcode & 0x07],
		mb -> cpu.ir,
		mb -> cpu.regs.accumulator,
		mb -> cpu.regs.zero_flag,
		mb -> cpu.regs.carry_flag);

}

//ROM storagment, debuggign LOL
static void mb_dump_rom(const motherboard_t *mb, uint8_t count){
	if (count > ROM64_BYTES) count = ROM64_BYTES;
	printf("\n ---ROM dump (0x00 ~ 0x%02X) ---\n", count - 1);
	for(uint16_t i = 0; i < count; i++) {
		if(i % 16 == 0) printf("0x%02X: ",i);
		printf("0x%02X", rom64_get_byte(&mb->rom,i));
		if(i % 16 == 15 || i == count -1)printf("\n");
	}
	printf("----------------------------------\n");
}

//rom storagement status
static void print_usage(const char *prog){
	printf("Usage: %s [hex_file] [max_ticks]\n", prog);
	printf("	hex_file 	ROM hex file to burn (default: program.hex)\n");
	printf("	max_ticks 	Number of clock cycles to run (default: %d)\n", MB_MAX_TICKS);
}	

//mainfunction: load hex -> execution
int main(int argc, char *argv[]){
	if (argc > 1 && ((strcmp(argc[1], "-h") == 0) || strcmp(argc[1], "--help") == 0)){
		print_usage(argc[0]);
		return 0;
	}
	
	//create the MB object
	motherboard_t mb;

	mb_init(&mb);

	//Burning the ROM 
	const char *hex_file = (argc > 1) ? argv[1] : "program.hex";
	int burned = rom64_load_hex_file(&mb.rom, hex_file, 0x00);
	if (burned < 0) {
		fprinf(stderr, "ERROR: cannot open hex file \"%s\"n", hex_file);
		return 1;
	}

	uint32_t max_ticks = (argc > 2) ? (uint32_t)atoi(argv[2]) : MB_MAX_TICKS;

	mb_dump_rom(&mb, (uint8_t)(burn < ROM64_BYTES ? burned : ROM64_BYTES));
	printf("=== 2N3904 TTL Computer - running %u ticks == \n\n", max_ticks);

	for (uint32_t i = 0; i < max_ticks; i++) {
		mb_ticks(&mb);
		if (mb.halted) {
			printf("\n **** HALT detectat tick %u *** \n", mb.tick_count);
			break;
		}
	}

	printf("\n === HALT after %u ticks === \n", mb.tick_count);
	printf("Final state: \n");
	printf("	ACC	= %u (0x%02X)\n", mb.cpu.regs.accumulator, mb.cpu.regs.accumulator);
	printf("	Z flag  = %d \n", mb.cpu.regs.zero_flag);
	printf("	C flag  = %d \n", mb.cpu.regs.carry_flag);
	printf("	Output  = %u (0x%02X)\n", mb.output_reg, mb.output_reg);
	printf("	PC      = 0x%02X\n", mb_pc_read(&mb));
	printf("------------------SRAM_DUMP-------------------\n");
	
	printf("----------------------------------------------\n");
	printf("THIS IS FINISHED, THX FOR RUNNING THIS PROGRAM");
}
