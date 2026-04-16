#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "74HC161.h"
#include "ROM_64B.h"
#include "RAM.h"

#define MB_PC_CHIPS  2
#define MB_MAX_TICKS 256 
#define MB_PC_MASK   0x00FF

typedef struct {
    cpu_t cpu;
    HC74_161 pc[MB_PC_CHIPS];
    ROM_64B rom;
    SRAM_8W ram;
    uint8_t main_bus;
    uint8_t output_reg;
    uint32_t tick_count;
    bool halted;
} motherboard_t;

static uint16_t mb_pc_read(const motherboard_t *mb) {
    return (uint16_t)(
        hc74_161_get_output((HC74_161 *)&mb->pc[0]) |    
        (hc74_161_get_output((HC74_161 *)&mb->pc[1]) << 4)
    );
}

static void mb_pc_clock_all(motherboard_t *mb, bool level) {
    for(int i = 0; i < MB_PC_CHIPS; i++) {
        hc74_161_set_clock(&mb->pc[i], level);
    }
} 

static void mb_pc_increment(motherboard_t *mb) {
    bool rco0 = hc74_161_get_rco(&mb->pc[0]);

    hc74_161_set_controls(&mb->pc[0], true, true, true, true);
    hc74_161_set_controls(&mb->pc[1], true, true, true, rco0);

    mb_pc_clock_all(mb, true);
    mb_pc_clock_all(mb, false);
}

static void mb_pc_load(motherboard_t *mb, uint16_t addr) {
    addr &= MB_PC_MASK;

    hc74_161_set_data(&mb->pc[0], addr & 0x0F);
    hc74_161_set_data(&mb->pc[1], (addr >> 4U) & 0x0F);

    for(uint8_t i = 0; i < MB_PC_CHIPS; i++) {
        hc74_161_set_controls(&mb->pc[i], true, false, true, true);
    }
    
    mb_pc_clock_all(mb, true);
    mb_pc_clock_all(mb, false);
    
    bool rco0 = hc74_161_get_rco(&mb->pc[0]);
    hc74_161_set_controls(&mb->pc[0], true, true, true, true);
    hc74_161_set_controls(&mb->pc[1], true, true, true, rco0);
}

static void mb_init(motherboard_t *mb) {
    memset(mb, 0, sizeof(*mb));

    for (uint8_t i = 0; i < MB_PC_CHIPS; i++) {
        hc74_161_init(&mb->pc[i]);
        hc74_161_set_controls(&mb->pc[i], false, true, false, false);
        hc74_161_set_controls(&mb->pc[i], true, true, true, true);
    }

    rom64_init(&mb->rom);
    rom64_reset(&mb->rom);
    sram8w_init(&mb->ram);
    cpu_init(&mb->cpu, &mb->main_bus);
}

static const char *opcode_names[] = {
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
    uint8_t pc = mb_pc_read(mb);

    mb->rom.addr = (uint8_t)(pc & ROM64_ADDR_MASK);
    mb->main_bus = rom64_read_selected(&mb->rom);

    cpu_notify_fetch_done(&mb->cpu);
    cpu_clock(&mb->cpu);

    if (cpu_get_out_strobe(&mb->cpu)) {
        mb->output_reg = mb->main_bus;
        printf("\t>>> OUT = 0x%02X (%u)\n", mb->output_reg, mb->output_reg);

        sram_8w_set_address(&mb->ram, mb->cpu.immediate & SRAM_ADDR_MASK);
        sram_8w_set_ctl(&mb->ram, true, false, true);
        sram_8w_write(&mb->ram, mb->cpu.regs.accumulator & SRAM_DATA_MASK);
        sram_8w_set_ctl(&mb->ram, false, false, false);
        printf("\t>>> SRAM[%u] <- 0x%02X\n",
                mb->cpu.immediate & SRAM_ADDR_MASK,
                mb->cpu.regs.accumulator & SRAM_DATA_MASK
        );
    }
    
    int16_t offset;
    if (cpu_get_jump_request(&mb->cpu, &offset)) {
        uint16_t new_pc = (uint16_t)((uint16_t)pc + offset) & MB_PC_MASK;
        printf("\t>>> JMP 0x%02X -> 0x%02X (offset %+d)\n", pc, new_pc, offset);
        mb_pc_load(mb, new_pc);
        cpu_clear_jump_request(&mb->cpu);
    } else {
        mb_pc_increment(mb);
    }

    mb->tick_count++;

    printf("[%4u] PC = 0x%02X, %s 0x%02X ACC = %2u z=%d C = %d\n",
        mb->tick_count, pc,
        opcode_names[mb->cpu.opcode & 0x07],
        mb->cpu.ir,
        mb->cpu.regs.accumulator,
        mb->cpu.regs.zero_flag,
        mb->cpu.regs.carry_flag);
}

static void mb_dump_rom(const motherboard_t *mb, uint8_t count) {
    if (count > ROM64_BYTES) count = ROM64_BYTES;
    printf("\n ---ROM dump (0x00 ~ 0x%02X) ---\n", count - 1);
    for(uint16_t i = 0; i < count; i++) {
        if(i % 16 == 0) printf("0x%02X: ", i);
        printf("%02X ", rom64_get_byte(&mb->rom, i));
        if(i % 16 == 15 || i == count - 1) printf("\n");
    }
    printf("----------------------------------\n");
}

static void print_usage(const char *prog) {
    printf("Usage: %s [hex_file] [max_ticks]\n", prog);
    printf("\thex_file \tROM hex file to burn (default: program.hex)\n");
    printf("\tmax_ticks \tNumber of clock cycles to run (default: %d)\n", MB_MAX_TICKS);
}    

int main(int argc, char *argv[]) {
    if (argc > 1 && ((strcmp(argv[1], "-h") == 0) || strcmp(argv[1], "--help") == 0)) {
        print_usage(argv[0]);
        return 0;
    }
    
    motherboard_t mb;
    mb_init(&mb);

    const char *hex_file = (argc > 1) ? argv[1] : "program.hex";
    int burned = rom64_load_hex_file(&mb.rom, hex_file, 0x00);
    if (burned < 0) {
        fprintf(stderr, "ERROR: cannot open hex file \"%s\"\n", hex_file);
        return 1;
    }

    uint32_t max_ticks = (argc > 2) ? (uint32_t)atoi(argv[2]) : MB_MAX_TICKS;

    mb_dump_rom(&mb, (uint16_t)((uint32_t)burned < (uint32_t)ROM64_BYTES ? (uint16_t)burned : (uint16_t)ROM64_BYTES));
    printf("=== 2N3904 TTL Computer - running %u ticks ===\n\n", max_ticks);

    for (uint32_t i = 0; i < max_ticks; i++) {
        mb_tick(&mb);
        if (mb.halted) {
            printf("\n **** HALT detected at tick %u *** \n", mb.tick_count);
            break;
        }
    }

    printf("\n === HALT after %u ticks ===\n", mb.tick_count);
    printf("Final state:\n");
    printf("\tACC\t= %u (0x%02X)\n", mb.cpu.regs.accumulator, mb.cpu.regs.accumulator);
    printf("\tZ flag\t= %d\n", mb.cpu.regs.zero_flag);
    printf("\tC flag\t= %d\n", mb.cpu.regs.carry_flag);
    printf("\tOutput\t= %u (0x%02X)\n", mb.output_reg, mb.output_reg);
    printf("\tPC\t= 0x%02X\n", mb_pc_read(&mb));
    printf("------------------SRAM DUMP-------------------\n");
    // 可以自行添加 SRAM 打印
    printf("----------------------------------------------\n");
    printf("THIS IS FINISHED, THX FOR RUNNING THIS PROGRAM\n");
    
    return 0;
}
