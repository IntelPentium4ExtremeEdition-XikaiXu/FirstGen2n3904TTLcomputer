#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define CPU_ACC_MASK 0x1F
#define CPU_INSTR_OPCODE_MASK 0xE0
#define CPU_INSTR_IMM5_MASK 0x1F 

typedef enum {
    OPCODE_NOP,
    OPCODE_LDA_imm5,
    OPCODE_ADD_imm5,
    OPCODE_SUB_imm5,
    OPCODE_LDM_addr3,
    OPCODE_SHR,
    OPCODE_JZ_imm5,
    OPCODE_OUT_imm5
} cpu_opcode_t;

typedef struct {        
    uint8_t accumulator;
    bool zero_flag;
    bool carry_flag;
} cpu_registers_t;

typedef struct {
    cpu_registers_t regs;
    uint8_t ir;
    uint8_t opcode;
    uint8_t immediate;
    uint8_t *main_bus;
    uint8_t cycle;
    bool fetch_complete;
    bool jump_request;
    int16_t jump_offset;
    bool out_strobe;
    bool load_strobe;
    uint8_t load_addr;
} cpu_t;    

static inline void cpu_init(cpu_t *cpu, uint8_t *bus_ptr) {
    cpu->regs.accumulator = 0;
    cpu->regs.zero_flag   = false;
    cpu->regs.carry_flag  = false;
    cpu->ir             = 0;
    cpu->opcode         = 0;
    cpu->immediate      = 0;
    cpu->main_bus       = bus_ptr;
    cpu->cycle          = 0;
    cpu->fetch_complete = false;
    cpu->jump_request   = false;
    cpu->jump_offset    = 0;
    cpu->out_strobe     = false;
    cpu -> load_strobe  = false;
    cpu -> load_addr    = 0;
}

static inline void cpu_reset(cpu_t *cpu) {
    cpu->regs.accumulator = 0;
    cpu->regs.zero_flag   = false;
    cpu->regs.carry_flag  = false;
    cpu->ir             = 0;
    cpu->cycle          = 0;
    cpu->jump_request   = false;
    cpu->out_strobe     = false;
    cpu->load_strobe    = false;
    cpu -> load_addr = 0;
}

static inline void cpu_alu_5bit(uint8_t a, uint8_t b, bool is_sub,
                                uint8_t *result, bool *carry_out) {
    uint8_t temp_a = a & CPU_ACC_MASK;
    uint8_t temp_b = b & CPU_ACC_MASK;

    if (is_sub) {
        *result = temp_a - temp_b;
        *carry_out = (temp_a < temp_b);
    } else {
        uint16_t sum = (uint16_t)temp_a + temp_b;
        *result = (uint8_t)sum;
        *carry_out = (sum > CPU_ACC_MASK);
    }
}

static inline void cpu_clock(cpu_t *cpu) {
    if (cpu->fetch_complete && cpu->cycle == 0) {
        cpu->ir        = *cpu->main_bus;
        cpu->opcode    = (cpu->ir >> 5) & 0x07;
        cpu->immediate = cpu->ir & CPU_INSTR_IMM5_MASK;
        cpu->cycle     = 1;
        cpu->fetch_complete = false;
        cpu->out_strobe = false;
    }

    if (cpu->cycle > 0) {
        switch (cpu->opcode) {
            case OPCODE_NOP:
                cpu->cycle = 0;
                break;
            case OPCODE_LDA_imm5:
                cpu->regs.accumulator = cpu->immediate;
                cpu->regs.zero_flag   = (cpu->immediate == 0);
                cpu->regs.carry_flag  = false;
                cpu->cycle = 0;
                break;
            case OPCODE_ADD_imm5:
            case OPCODE_SUB_imm5: {
                uint8_t result;
                bool carry;
                cpu_alu_5bit(cpu->regs.accumulator, cpu->immediate,
                             (cpu->opcode == OPCODE_SUB_imm5),
                             &result, &carry);
                cpu->regs.accumulator = result;
                cpu->regs.zero_flag   = (result == 0);
                cpu->regs.carry_flag  = carry;
                cpu->cycle = 0;
                break;
            }
            case OPCODE_LDM_addr3:
                cpu->regs.accumulator &= cpu->immediate;
                cpu->regs.zero_flag   = (cpu->regs.accumulator == 0);
                cpu->regs.carry_flag  = false;
                cpu->cycle = 0;
                break;
            case OPCODE_SHR:
                cpu->regs.carry_flag  = cpu->regs.accumulator & 1;
                cpu->regs.accumulator >>= 1;
                cpu->regs.zero_flag   = (cpu->regs.accumulator == 0);
                cpu->cycle = 0;
                break;
            case OPCODE_JZ_imm5:
                if (cpu->regs.zero_flag) {
                    int8_t signed_imm = (int8_t)(cpu->immediate << 3) >> 3;
                    cpu->jump_offset  = (int16_t)signed_imm;
                    cpu->jump_request = true;
                }
                cpu->cycle = 0;
                break;
            case OPCODE_OUT_imm5:
                *cpu->main_bus = cpu->regs.accumulator;
                cpu->out_strobe = true;
                cpu->cycle = 0;
                break;
            default:
                cpu->cycle = 0;
                break;
        }
    }
}

static inline void cpu_notify_fetch_done(cpu_t *cpu) {
    cpu->fetch_complete = true;
}

static inline bool cpu_get_out_strobe(const cpu_t *cpu) {
    return cpu->out_strobe;
}

static inline bool cpu_get_jump_request(const cpu_t *cpu, int16_t *offset) {
    if (cpu->jump_request) {
        *offset = cpu->jump_offset;
        return true;
    }
    return false;
}

static inline void cpu_clear_jump_request(cpu_t *cpu) {
    cpu->jump_request = false;
    cpu->jump_offset  = 0;
}

#endif
