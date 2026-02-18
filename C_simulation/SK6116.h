#ifndef SK6116_H
#define SK6116_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>  // 用于随机初始化

// SK6116 / 6116 系列 SRAM 模拟（更真实风格）
// 容量：2K × 8 bit = 2048 字节
// 地址线：A0 ~ A10（11根）
// 数据线：DQ0 ~ DQ7（双向 I/O，与 CPU 交互）
// 控制信号：
//   /CE   芯片使能（低有效）
//   /OE   输出使能（低有效，读时用）
//   /WE   写入使能（低有效，写时用）
//
// 真实风格改进：
// - 上电初始化为随机值（模拟真实 SRAM 上电不确定性）
// - 双向数据总线：CPU 驱动写入（set_data），SRAM 驱动读取（get_data）
// - 异步操作：无时钟、无延迟，条件满足即读/写
// - 动态监控：提供 dump 函数，可在模拟循环中调用，输出当前 SRAM 内容到 TXT 文件
//   （格式：每行一个字节 "addr 0xXXXX: 0xYY"，便于监控变化）
// - 无文件预加载：纯模拟，内容由 CPU 运行时 I/O 写入

#define SK6116_SIZE      2048
#define SK6116_ADDR_MASK 0x07FF   // 低11位有效（0~2047）

typedef struct {
    uint8_t sram[SK6116_SIZE];    // SRAM 存储数组（随机初始化）

    bool CE_n;                    // /CE 低有效（true=高=禁用）
    bool OE_n;                    // /OE 低有效（true=高=禁用）
    bool WE_n;                    // /WE 低有效（true=高=禁用）

    uint16_t address;             // 当前地址（A0~A10）
    uint8_t  data_bus;            // 数据总线（双向，与 CPU 交互）

} SK6116;

// 初始化 SRAM 芯片（随机填充内容，模拟上电随机）
void sk6116_init(SK6116 *chip);

// 复位控制信号和总线状态（不改变 SRAM 内容）
void sk6116_rst(SK6116 *chip);

// 设置地址（只取低11位，防止溢出）
void sk6116_set_address(SK6116 *chip, uint16_t addr);

// 设置控制信号（传入 true=使能=低电平）
void sk6116_set_controls(SK6116 *chip, bool CE, bool OE, bool WE);

// 设置数据总线（CPU 驱动，用于写入；如果写条件满足，立即写入 SRAM）
void sk6116_set_data(SK6116 *chip, uint8_t data);

// 获取当前数据总线值（SRAM 驱动，用于读取；如果读条件满足，从 SRAM 输出）
uint8_t sk6116_get_data(SK6116 *chip);

// 动态监控：输出当前 SRAM 全部内容到 TXT 文件（每行 "addr 0xXXXX: 0xYY"）
// 可在模拟循环中（如每次写入后）调用，实现变化监控
void sk6116_dump_to_txt(SK6116 *chip, const char *filename);

// ────────────────────────────────────────────────
// 函数实现
// ────────────────────────────────────────────────

void sk6116_init(SK6116 *chip) {
    // 模拟真实上电：随机初始化 SRAM 内容
    srand((unsigned int)time(NULL));  // 随机种子
    for (int i = 0; i < SK6116_SIZE; i++) {
        chip->sram[i] = (uint8_t)(rand() % 256);
    }
    
    sk6116_rst(chip);
}

void sk6116_rst(SK6116 *chip) {
    chip->CE_n = true;     // 初始禁用（高电平）
    chip->OE_n = true;
    chip->WE_n = true;
    
    chip->address  = 0;
    chip->data_bus = 0xFF; // 高阻态默认值
}

void sk6116_set_address(SK6116 *chip, uint16_t addr) {
    chip->address = addr & SK6116_ADDR_MASK;
}

void sk6116_set_controls(SK6116 *chip, bool CE, bool OE, bool WE) {
    chip->CE_n = !CE;   // true → /CE = 低（使能）
    chip->OE_n = !OE;
    chip->WE_n = !WE;
    
    // SRAM 异步：控制信号变化后，读/写在 get/set_data 中判断
}

void sk6116_set_data(SK6116 *chip, uint8_t data) {
    chip->data_bus = data;
    
    // 写条件：/CE 低 + /WE 低（/OE 可任意，但通常高）
    if (!chip->CE_n && !chip->WE_n) {
        chip->sram[chip->address] = chip->data_bus;
        // 可选：在这里自动 dump 到文件（但为灵活性，建议外部调用）
    }
}

uint8_t sk6116_get_data(SK6116 *chip) {
    // 读条件：/CE 低 + /OE 低 + /WE 高
    if (!chip->CE_n && !chip->OE_n && chip->WE_n) {
        chip->data_bus = chip->sram[chip->address];
    } else {
        // 否则高阻态
        chip->data_bus = 0xFF;
    }
    
    return chip->data_bus;
}

void sk6116_dump_to_txt(SK6116 *chip, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        // 错误处理：打印到 stderr（模拟环境中）
        fprintf(stderr, "无法打开文件 %s\n", filename);
        return;
    }
    
    // 输出 2048 行，每行一个字节
    for (int addr = 0; addr < SK6116_SIZE; addr++) {
        fprintf(f, "addr 0x%04X: 0x%02X\n", addr, chip->sram[addr]);
    }
    
    fclose(f);
}

#endif