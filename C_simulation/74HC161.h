#ifndef HC74_161_H
#define HC74_161_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 74HC161 4位同步二进制计数器模拟（用作程序计数器 PC）
// 功能：4位计数（0~15），可同步加载、异步清零、计数使能、进位输出
// 引脚：
//   CLK   时钟输入（上升沿触发）
//   /CLR  异步清零（低有效，清零到 0000）
//   /LOAD 同步加载（低有效，在 CLK 上升沿加载 D 输入）
//   ENP   并行使能（高有效，与 ENT 一起控制计数）
//   ENT   终端使能（高有效，与 ENP 一起控制计数，且产生 RCO）
//   D0~D3 数据输入（用于加载值）
//   Q0~Q3 计数输出
//   RCO   进位输出（当计数=15 且 ENT=1 时为高）
//
// 真实风格：
// - 异步清零优先
// - 同步加载/计数在 CLK 上升沿
// - 作为 PC：通常 /LOAD=高（不加载），ENP/ENT=高（使能计数），CLK 来自 CPU 时钟
// - 纯模拟：无文件加载，内容由输入动态变化
// - 动态监控：提供 dump 函数，输出当前状态到 TXT 文件（便于监控 PC 值变化）

#define HC74_161_BITS 4   // 4位计数器

typedef struct {
    uint8_t Q;            // 当前计数输出（Q3~Q0，低4位有效，0~15）

    bool CLK;             // 时钟输入（true=高）
    bool CLR_n;           // /CLR 低有效（true=高=无效）
    bool LOAD_n;          // /LOAD 低有效（true=高=无效）
    bool ENP;             // ENP 高有效（true=使能）
    bool ENT;             // ENT 高有效（true=使能）

    uint8_t D;            // 数据输入（D3~D0，低4位）

    bool RCO;             // 进位输出（true=高）

    // 内部：记录上一个 CLK 状态，用于检测上升沿
    bool prev_CLK;

} HC74_161;

// 初始化计数器（默认清零）
void hc74_161_init(HC74_161 *chip);

// 复位所有引脚状态（不改变 Q 值）
void hc74_161_rst(HC74_161 *chip);

// 设置数据输入（D3~D0，低4位）
void hc74_161_set_data(HC74_161 *chip, uint8_t data);

// 设置控制信号（CLR_n, LOAD_n, ENP, ENT 传入 true=高/无效/使能）
void hc74_161_set_controls(HC74_161 *chip, bool CLR_n, bool LOAD_n, bool ENP, bool ENT);

// 设置/触发时钟（传入 true=高，内部检测上升沿）
void hc74_161_set_clock(HC74_161 *chip, bool CLK);

// 获取当前输出（返回 Q 的值，0~15）
uint8_t hc74_161_get_output(HC74_161 *chip);

// 获取进位输出（true=高）
bool hc74_161_get_rco(HC74_161 *chip);

// 动态监控：输出当前状态到 TXT 文件（"Q: 0xXX, RCO: X" 等）
void hc74_161_dump_to_txt(HC74_161 *chip, const char *filename);

// ────────────────────────────────────────────────
// 函数实现
// ────────────────────────────────────────────────

void hc74_161_init(HC74_161 *chip) {
    chip->Q = 0;          // 初始清零
    
    hc74_161_rst(chip);
}

void hc74_161_rst(HC74_161 *chip) {
    chip->CLK = false;
    chip->CLR_n = true;   // 初始无效（高）
    chip->LOAD_n = true;  // 初始无效（高）
    chip->ENP = false;    // 初始禁用
    chip->ENT = false;    // 初始禁用
    
    chip->D = 0;
    
    chip->RCO = false;
    
    chip->prev_CLK = false;
}

void hc74_161_set_data(HC74_161 *chip, uint8_t data) {
    chip->D = data & 0x0F;  // 只取低4位
}

void hc74_161_set_controls(HC74_161 *chip, bool CLR_n, bool LOAD_n, bool ENP, bool ENT) {
    chip->CLR_n = CLR_n;
    chip->LOAD_n = LOAD_n;
    chip->ENP = ENP;
    chip->ENT = ENT;
    
    // 异步清零：如果 /CLR = 低（CLR_n=false），立即清零
    if (!chip->CLR_n) {
        chip->Q = 0;
        chip->RCO = false;
    }
}

void hc74_161_set_clock(HC74_161 *chip, bool CLK) {
    chip->prev_CLK = chip->CLK;
    chip->CLK = CLK;
    
    // 检测上升沿（从低到高）
    if (!chip->prev_CLK && chip->CLK) {
        // 先检查异步清零（但已在上层处理，这里冗余检查）
        if (!chip->CLR_n) {
            chip->Q = 0;
            chip->RCO = false;
            return;
        }
        
        // 同步操作
        if (!chip->LOAD_n) {
            // /LOAD 低：加载 D
            chip->Q = chip->D;
        } else if (chip->ENP && chip->ENT) {
            // 使能计数：Q + 1（模16）
            chip->Q = (chip->Q + 1) & 0x0F;
        }
        
        // 更新 RCO：当 Q=15 且 ENT=1 时，高
        chip->RCO = (chip->Q == 0x0F) && chip->ENT;
    }
}

uint8_t hc74_161_get_output(HC74_161 *chip) {
    return chip->Q;
}

bool hc74_161_get_rco(HC74_161 *chip) {
    return chip->RCO;
}

void hc74_161_dump_to_txt(HC74_161 *chip, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "无法打开文件 %s\n", filename);
        return;
    }
    
    // 输出当前状态
    fprintf(f, "Q (output): 0x%01X (%d)\n", chip->Q, chip->Q);
    fprintf(f, "RCO (carry out): %d\n", chip->RCO);
    fprintf(f, "CLK: %d\n", chip->CLK);
    fprintf(f, "CLR_n: %d\n", chip->CLR_n);
    fprintf(f, "LOAD_n: %d\n", chip->LOAD_n);
    fprintf(f, "ENP: %d\n", chip->ENP);
    fprintf(f, "ENT: %d\n", chip->ENT);
    fprintf(f, "D (input): 0x%01X\n", chip->D);
    
    fclose(f);
}

#endif