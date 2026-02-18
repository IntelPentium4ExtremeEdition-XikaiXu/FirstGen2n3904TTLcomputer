module adder_8bit (
    input  logic [4:0] inputA,
    input  logic [4:0] inputB,
    input  logic       cin,
    output logic [4:0] outputY,
    output logic       cout
);

//The internal connection inside of the Full adder system, 5 bit sel
wire c1, c2, c3, c4;
singlebit_adder a0, a1, a2, a3, a4;

    a0 (
        .x   (inputA[0]),
        .y   (inputB[0]),
        .cin (cin),
        .ans (outputY[0]),
        .cout(c1)
    );

    a1 (
        .x   (inputA[1]),
        .y   (inputB[1]),
        .cin (c1),
        .ans (outputY[1]),
        .cout(c2)
    );

    a2 (
        .x   (inputA[2]),
        .y   (inputB[2]),
        .cin (c2),
        .ans (outputY[2]),
        .cout(c3)
    );

    a3 (
        .x   (inputA[3]),
        .y   (inputB[3]),
        .cin (c3),
        .ans (outputY[3]),
        .cout(c4)
    );

    a4 (
        .x   (inputA[4]),
        .y   (inputB[4]),
        .cin (c4),
        .ans (outputY[4]),
        .cout(cout)
    );
endmodule

