//============================================================
// AT28C64 - 8K x 8 EEPROM 行为模型（用于CPU/TTL仿真）
// 支持从 hex/txt 文件加载 ROM 内容 - each Row hex val 
//============================================================
module at28c64_rom (
    input  logic        ce_n,     // Chip Enable，低有效
    input  logic        oe_n,     // Output Enable，低有效
    input  logic        we_n,     // Write Enable（本模型忽略写）
    input  logic [12:0] addr,     // A0-A12
    inout  wire  [7:0]  data      // D0-D7 三态总线
);

    //--------------------------------------------------------
    // 8K x 8 存储阵列
    //--------------------------------------------------------
    logic [7:0] mem [0:8191];

    //--------------------------------------------------------
    // 初始化：从文件加载 ROM 内容
    // 文件名可改
    //--------------------------------------------------------
    initial begin
        $display("Loading ROM content from rom.hex ...");
        $readmemh("rom.hex", mem);
    end

    //--------------------------------------------------------
    // 读操作（AT28C64 读是组合逻辑）
    //--------------------------------------------------------
    logic [7:0] data_out;

    always_comb begin
        if (!ce_n && !oe_n && we_n) begin
            data_out = mem[addr];
        end else begin
            data_out = 8'hZZ;  // 高阻
        end
    end

    assign data = data_out;

endmodule

//example coide in the rom.hex:
//00
//11
//22
//33
//44
//55
//66
//77
//88
//99
//AA
//BB
//CC
//DD
//EE
//FF
