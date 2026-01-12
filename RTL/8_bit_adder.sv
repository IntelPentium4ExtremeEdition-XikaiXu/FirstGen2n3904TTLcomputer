module adder_8bit (
    input  logic [7:0] inputA,
    input  logic [7:0] inputB,
    input  logic       cin,
    output logic [7:0] outputY,
    output logic       cout
);
    logic c1, c2, c3, c4, c5, c6, c7;

    singlebit_adder a0 (
        .x   (inputA[0]),
        .y   (inputB[0]),
        .cin (cin),
        .ans (outputY[0]),
        .cout(c1)
    );

    singlebit_adder a1 (
        .x   (inputA[1]),
        .y   (inputB[1]),
        .cin (c1),
        .ans (outputY[1]),
        .cout(c2)
    );

    singlebit_adder a2 (
        .x   (inputA[2]),
        .y   (inputB[2]),
        .cin (c2),
        .ans (outputY[2]),
        .cout(c3)
    );

    singlebit_adder a3 (
        .x   (inputA[3]),
        .y   (inputB[3]),
        .cin (c3),
        .ans (outputY[3]),
        .cout(c4)
    );

    singlebit_adder a4 (
        .x   (inputA[4]),
        .y   (inputB[4]),
        .cin (c4),
        .ans (outputY[4]),
        .cout(c5)
    );

    singlebit_adder a5 (
        .x   (inputA[5]),
        .y   (inputB[5]),
        .cin (c5),
        .ans (outputY[5]),
        .cout(c6)
    );

    singlebit_adder a6 (
        .x   (inputA[6]),
        .y   (inputB[6]),
        .cin (c6),
        .ans (outputY[6]),
        .cout(c7)
    );

    singlebit_adder a7 (
        .x   (inputA[7]),
        .y   (inputB[7]),
        .cin (c7),
        .ans (outputY[7]),
        .cout(cout)
    );

endmodule

