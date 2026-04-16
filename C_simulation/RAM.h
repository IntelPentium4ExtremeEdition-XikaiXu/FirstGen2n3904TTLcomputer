#ifndef SRAM_H
#define SRAM_H

#include <stdint.h>
#include <stdbool.h>

#define SRAM_WORDS     8u
#define SRAM_BITS      5u
#define SRAM_DATA_MASK 0x1Fu
#define SRAM_ADDR_BITS 3u
#define SRAM_ADDR_MASK 0x7u
#define SRAM_8B_SIZE   SRAM_WORDS

typedef struct {
    bool Q;
} storage_cell_t;

typedef struct {
    storage_cell_t cells[SRAM_WORDS][SRAM_BITS];
    bool CE_n;
    bool OE_n;
    bool WE_n;
    uint8_t addr;
    uint8_t data_bus;
} SRAM_8W;

static inline void sram8w_init(SRAM_8W *ram) {
    for(uint8_t row = 0; row < SRAM_WORDS; ++row) {
        for(uint8_t bit = 0; bit < SRAM_BITS; ++bit) {
            ram->cells[row][bit].Q = false;
        }
    }
    ram->CE_n      = true;
    ram->OE_n      = true;
    ram->WE_n      = true;
    ram->addr      = 0U;
    ram->data_bus  = 0x1Fu;
}

static inline void sram_8w_set_address(SRAM_8W *ram, uint16_t addr) {
    ram->addr = (uint8_t)(addr & SRAM_ADDR_MASK);
}

static inline void sram_8w_set_ctl(SRAM_8W *ram, bool CE, bool OE, bool WE) {
    ram->CE_n = !CE;
    ram->OE_n = !OE;
    ram->WE_n = !WE;
}

static inline void sram_8w_write(SRAM_8W *ram, uint8_t data) {
    ram->data_bus = data;
    if (!ram->CE_n && !ram->WE_n) {
        uint8_t a = (uint8_t)(ram->addr & SRAM_ADDR_MASK);
        uint8_t masked = (uint8_t)(data & SRAM_DATA_MASK);
        for(uint8_t bit = 0; bit < SRAM_BITS; ++bit) {
            ram->cells[a][bit].Q = ((masked >> bit) & 1u) != 0u;
        }
    }
}

static inline uint8_t sram_8w_get_data(SRAM_8W *ram) {
    if(!ram->CE_n && !ram->OE_n && ram->WE_n) {
        uint8_t a = (uint8_t)(ram->addr & SRAM_ADDR_MASK);
        uint8_t val = 0u;
        for(uint8_t bit = 0; bit < SRAM_BITS; ++bit) {
            if(ram->cells[a][bit].Q) {
                val |= (uint8_t)(1u << bit);
            }
        }
        ram->data_bus = val;
    } else {
        ram->data_bus = 0x1Fu;
    }
    return ram->data_bus;
}

#endif