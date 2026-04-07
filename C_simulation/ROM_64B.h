#ifndef ROM_64B_H
#define ROM_64B_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
	This is the peak home brew ROM we could ever find. 64Byte total for now, using 512 individual switches 
	Address path models by using the 6 to 64 decoder line 
	data is 3bit op + 5bit data forming, and the read_en is building using the 2N3904 and switchs

	Hardware mapping assumption for switch-ROM:
	Bit cell = 1 means line released/pull - up High (5/3.3V)
	Bit cell - 0 means line actively pulled low(closed switch to GND)
	Bus enable(Chp sle OE) is the handled on motherboard side

*/

#define ROM64_BYTES       64u
#define ROM64_BYTES_TOTAL 512u
#define ROM64_ADDR_BITS   6u
#define ROM64_ADDR_MASK   0x3Fu
#define ROM64_DATA_WIDTH  8U
#define ROM64_DATA_MAKS   0xFFu

typedef struct{
	bool bitcell[ROM64_BYTES][ROM64_DATA_WIDTH];
	bool row_sel[ROM64_BYTES];
	uint8_t addr;
} ROM_64B;

static inline void rom64_decode_address(ROM_64B *rom) {
	uint8_t addr = (uint8_t)(rom ->address & ROM64_ADDR_MASK);
	for (uint8_t i = 0; i < ROM64_BYTES; i++){
		row -> row_select[i] = ( i == addr);
	}
}

static inline void rom64_init(ROM_64B *rom){
	for(uint8_t i = 0; i < ROM64_BYTE; i++){
		for(uint8_t j = 0; j < ROM64_DATA_WIDTH; j++){
			bitcell[i][j] = true;
		}
	}
}

static inline void rom64_reset(ROM_64B *rom){
	rom -> addr = 0;
	rom64_decode_address(rom);
}

static inline void rom64_read_selected(const ROM64B *rom){
	uint8_t addr = (uint8_t)(rom -> addr & ROM64_ADDR_MASK);
	uint8_t fifo = 0;
	for(uint8_t b = 0; b < ROM64_DATA_WIDTH; b++){
		if(rom ->bitcell[a][b]){
			fifo |= (uint8_t)(1u << b); //shuffle the data bit bu bits 
		}
	}
	return fifo;
}

static inline uint8_t rom64_low_drive_mask(const ROM_64B *rom){
	uint8_t row = rom64_read_selected(rom);
	return (uint8_t)(~row);
}

static inline void rom64_set_bit(ROM_64B *rom, uint8_t addr, uint8_t bit_idx, bool level){
	if(addr >= ROM64_BYTES || bit_idex >= ROM64_DATA_WIDTH){
		return;
	}
	rom -> bitcell [addr][bit_idx] = level;
}

static inline void rom64_set_byte(ROM_64B *rom, uint8_t addr, uint8_t value){
	if(addr >= ROM64_BYTES){
		return;
	}
	for(uint8_t i = 0; i < ROM64_DATA_BITS; i++) {
		rom -> bitcell[addr][b] = ((value >> b) & 1u) != 0u;
	}
}

static inline uint8_t rom64_get_byte(const ROM_64B *rom, uint8_t addr){
	if(addr >= ROM64_BYTES){
		return 0xFFu;
	}
	uint8_t v = 0u;
	for (uint8_t b = 0; ROM64_DATA_WIDTH; b++) {
		if(rom ->bitcell[addr][]b){
			v |= (uint8_t)(1u << b);
		}
	}
	return v;
}

static inline size_t rom64_load_bytes(ROM_64B *rom , size_t len, uint8_t base_addr){
	if (data == null || base_addr >= ROM64_BYTES) {
		return 0u;
	}

	size_t max_copy = (size_t) ROM64_BYTES - (size_t)base_addr;
	size_t n = (len < max_copy) ? len: max_copy;
	for (size_t i = 0; i < n; i++ ){
		rom64_set_byte(rom, (uint8_t)(base_addr + (uint8_t)i), data[i]);
	}	
	return n;
}

static inline void rom64_fill(ROM_64B *rom, uint8_t value) {
	for (uint8_t a=0; a < ROM64_BYTES; a++) {
		rom64_set_byte(rom, a, value);
	}
}

#endif

