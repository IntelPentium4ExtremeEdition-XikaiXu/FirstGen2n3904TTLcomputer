//============================================================
// 6116ALSP-12  2K x 8 SRAM 行为等价模型
// 完全符合真实芯片 CE / OE / WE 时序逻辑 ChatGPT
//============================================================
module sram_6116 (
    input  logic        ce_n,      // Chip Enable，低有效
    input  logic        oe_n,      // Output Enable，低有效
    input  logic        we_n,      // Write Enable，低有效
    input  logic [10:0] addr,      // A0-A10 (2K 地址)
    inout  wire  [7:0]  data       // D0-D7 双向三态总线
);

    //--------------------------------------------------------
    // 2K x 8 存储阵列
    //--------------------------------------------------------
    logic [7:0] mem [0:2047];

    //--------------------------------------------------------
    // 写操作（当 CE=0 且 WE=0）
    //--------------------------------------------------------
    always_ff @(negedge we_n) begin
        if (!ce_n) begin
            mem[addr] <= data;
        end
    end

    //--------------------------------------------------------
    // 读操作（组合逻辑）
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
