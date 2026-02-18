//============================================================
// 74HC161 - 4-bit Synchronous Binary Counter / Program Counter
// 完全行为等价模型 by the CHATGPT
//============================================================
module hc161_pc (
    input  logic        clk,      // 上升沿触发
    input  logic        clr_n,    // 同步清零，低有效
    input  logic        load_n,   // 同步加载，低有效
    input  logic        enp,      // 计数使能 P
    input  logic        ent,      // 计数使能 T
    input  logic [3:0]  d,        // 并行输入
    output logic [3:0]  q,        // 输出
    output logic        rco       // 进位输出
);

    //--------------------------------------------------------
    // 同步逻辑：完全按 74HC161 优先级
    //--------------------------------------------------------
    always_ff @(posedge clk) begin
        if (!clr_n) begin
            // 优先级最高：同步清零
            q <= 4'b0000;
        end
        else if (!load_n) begin
            // 第二优先级：并行加载（用于 Jump）
            q <= d;
        end
        else if (enp && ent) begin
            // 第三优先级：计数
            q <= q + 1'b1;
        end
        else begin
            // 否则保持
            q <= q;
        end
    end

    //--------------------------------------------------------
    // RCO (Ripple Carry Output)
    // 当计数到 1111 且 ENT=1 时输出高
    //--------------------------------------------------------
    assign rco = ent && (q == 4'b1111);

endmodule
