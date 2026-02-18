`timescale 1ns/1ps

module tb_5_bit_adder;

    // -------------------------
    // 信号定义
    // -------------------------
    logic [4:0] a;
    logic [4:0] b;
    logic       cin;

    logic [4:0] sum;
    logic       cout;

    // -------------------------
    // DUT 实例化
    // -------------------------
    five_bit_adder uut (
        .a   (a),
        .b   (b),
        .cin (cin),
        .sum (sum),
        .cout(cout)
    );

    // -------------------------
    // 测试过程
    // -------------------------
    initial begin

        $display("Starting 5-bit adder test...");
        $display("Time |  A   +  B   + Cin |  Sum  Cout");

        // 基础测试
        a = 5'd0;  b = 5'd0;  cin = 0; #10;
        print_state();

        a = 5'd5;  b = 5'd3;  cin = 0; #10;
        print_state();

        a = 5'd10; b = 5'd7;  cin = 0; #10;
        print_state();

        a = 5'd15; b = 5'd15; cin = 0; #10;
        print_state();

        // 测试 carry in
        a = 5'd8;  b = 5'd8;  cin = 1; #10;
        print_state();

        // 测试溢出
        a = 5'd31; b = 5'd1;  cin = 0; #10;
        print_state();

        // 随机测试
        repeat (20) begin
            a   = $urandom_range(0,31);
            b   = $urandom_range(0,31);
            cin = $urandom_range(0,1);
            #10;
            print_state();
        end
	$dumpfile("5BIT_FULL_ADDER.vcd");
    	$dumpvars(0, 5_BIT_FULL_ADDER_TB);
        $display("Test finished.");
        $finish;
    end


    // -------------------------
    // 打印任务
    // -------------------------
    task print_state;
        begin
            $display("%4t | %2d + %2d + %1d  |  %2d   %1d",
                     $time, a, b, cin, sum, cout);

            // 自动检查
            if ({cout, sum} !== (a + b + cin)) begin
                $display("ERROR: Mismatch detected!");
            end
        end
    endtask

endmodule
