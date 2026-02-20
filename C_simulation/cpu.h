#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
/* @file cpu.h
 * @brief DIY 8 bit NOR logic cpu simulation
 * This is the homebrew CPU
 * after mass amount of thinking and make tons of disicion, sel ins + data arch
 * 3 bit ins + 5 bit data, total 8 bit length
 * PC is 12 bit total
 * 3 cycle file the addr,  8 instructions , 2N3902 NOR GATE BASED
 * Jack Xu, 2025-06-01 to pending 
 */
#ifndef CPU_H
#define CPU_H
#define CPU_ACC_MASK 0x1F
#define CPU_INSTR_OPCODE_MASK 0xE0
#define CPU_INSTR_IMM5_MASK 0x1F 

/**
 * @brief CPU total support maxout 8 instrcutions, 3 bit wide decoding mech
 * total instructions
 */
typedef enum{
    NOP,      	    //000 - null processing, do nothing, used for filling, aligned, timing lentancy process - null flag - null op
    LDA_imm5        //001 - init ACC register, setting default value - zero flag(if imm5 == 0, z = 1) - acc <- imm5
    ADD_imm5,       //010 - acculmate, PC counter increment - Z/C - acc <- acc + imm5
    SUB_imm5,       //011 - Subtraction, comparision - Z/C - acc <- acc - imm5
    AND_imm5,       //100 - bit masking, testing certain bit, clear bit - Z/C - acc <- acc & imm5
    SHR,            //101 - reg left shifing, used for driving the lcd/dividing  - Z/C - acc <- acc >> 1
    JZ_imm5,        //110 - statement jump, if statement ACC = 0 detecting - null flag - if Z =1, PC <- PC + sign_extend(imm5)
    OUT_imm5        //111 - ACC to the mainbus, sending the data through this inst - null flag - mainbus <- acc 
} cpu_opcode_t;

/**
 * @brief: internal of cpu only have 3 registers
 */ 
typedef struct {        
    uint8_t Acculmator;           //only use 5 bit of it - real 
    bool zero_flag;               //zero flag register - real
    bool carry_flag;              //carry flag register - real
} cpu_registers_t;

/**
 * @brief cpu logic itself. contains control unit and fake simulation registers
 */
typedef struct {
	//registers which connected to the system
	cpu_registers_t regs;

	//current instrcutions and decode result
	uint8_t ir; 		//instrcutions register, input from main_bus - fake
	uint8_t opcode;		//opcode detect, came from main bus - fake 	
	uint8_t immediate;      //decoded the 5 bit immediate val, came from main_bus - fake

	//main connection interface, driven by outside world
	uint8_t *main_bus;      //data bus, pointed to the outside

	//cycle statement
	uint8_t cycle; 		//current instruction clock consumption
	bool fetch_complete;	//finished the instruction decode
	bool jump_request;	//check do we have the JZ statement
	int16_t jump_offset;    //calculate the ofst we need to get
	//control signal
	bool out_strobe;	//out instruction told motherboard doing someshit, used for opcode gen sudden control signal, used for sram WE_n

} cpu_t;	

/* ────────────────────────────────────────────────
 * function implientation 全部放在 
 * ──────────────────────────────────────────────── */

/**
 * @brief init CPU core（not include PC, sram, etc）
 * @param 
 * @param bus_ptr the pinter points from the outside bus（from  motherboard.c）
 */
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
}

/**
 * @brief CPU rst（only inner condition）
 */

static inline void cpu_reset(cpu_t *cpu) {
    cpu->regs.accumulator = 0;
    cpu->regs.zero_flag   = false;
    cpu->regs.carry_flag  = false;
    cpu->ir             = 0;
    cpu->cycle          = 0;
    cpu->jump_request   = false;
    cpu->out_strobe     = false;
}

/**
 * @brief 簡單 5 位 ALU（加/減）
 * @param a         ACC 值（已遮罩）
 * @param b         立即數（已遮罩）
 * @param is_sub    是否減法
 * @param result    計算結果
 * @param carry_out 進位/借位輸出
 */
static inline void cpu_alu_5bit(uint8_t a, uint8_t b, bool is_sub,
                                uint8_t *result, bool *carry_out) {
    uint8_t temp_a = a & CPU_ACC_MASK;
    uint8_t temp_b = b & CPU_ACC_MASK;

    if (is_sub) {
        *result = temp_a - temp_b;
        *carry_out = (temp_a < temp_b);           // 借位
    } else {
        uint16_t sum = (uint16_t)temp_a + temp_b;
        *result = (uint8_t)sum;
        *carry_out = (sum > CPU_ACC_MASK);        // 進位
    }
}

/**
 * @brief 單步執行一個時鐘週期（由 motherboard.c 呼叫）
 * @param cpu CPU 實例
 */
static inline void cpu_clock(cpu_t *cpu) {
    // ── 階段 1：if we just finished fetching the data, decode ───────────────────────────────
    if (cpu->fetch_complete && cpu->cycle == 0) {
        cpu->ir        = *cpu->main_bus;
        cpu->opcode    = (cpu->ir >> 5) & 0x07;
        cpu->immediate = cpu->ir & CPU_INSTR_IMM5_MASK;
        cpu->cycle     = 1;
        cpu->fetch_complete = false;
        cpu->out_strobe = false;
    }

    // ── 階段 2：執行階段 ────────────────────────────────────────────────
    if (cpu->cycle > 0) {
        switch (cpu->opcode) {
            case OPCODE_NOP:
                // 什麼都不做
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

            case OPCODE_AND_imm5:
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
                    // 計算符號擴展偏移（5位 → 13位有符號，但我們用 12位 PC）
                    int8_t signed_imm = (int8_t)(cpu->immediate << 3) >> 3; // 符號擴展
                    cpu->jump_offset  = (int16_t)signed_imm;
                    cpu->jump_request = true;
                }
                cpu->cycle = 0;
                break;

            case OPCODE_OUT_imm5:
                *cpu->main_bus = cpu->regs.accumulator;
                cpu->out_strobe = true;   // 告訴母板：輸出有效，可鎖存到移位寄存器
                cpu->cycle = 0;
                break;

            default:
                // 非法指令，什麼都不做
                cpu->cycle = 0;
                break;
        }
    }
}

/**
 * @brief 通知 CPU：取指完成（由母板在 ROM 讀取後呼叫）
 * @param cpu CPU 實例
 */
static inline void cpu_notify_fetch_done(cpu_t *cpu) {
    cpu->fetch_complete = true;
}

/**
 * @brief 取得當前輸出脈衝（給 OUT 指令用）
 */
static inline bool cpu_get_out_strobe(const cpu_t *cpu) {
    return cpu->out_strobe;
}

/**
 * @brief 取得跳轉請求與偏移（母板用來控制 PC 載入）
 */
static inline bool cpu_get_jump_request(const cpu_t *cpu, int16_t *offset) {
    if (cpu->jump_request) {
        *offset = cpu->jump_offset;
        return true;
    }
    return false;
}

/**
 * @brief 清除跳轉請求（母板完成跳轉後呼叫）
 */
static inline void cpu_clear_jump_request(cpu_t *cpu) {
    cpu->jump_request = false;
    cpu->jump_offset  = 0;
}

#endif /* CPU_H */
