module singlebit_adder (
    input  logic x,
    input  logic y,
    input  logic cin,
    output logic ans,
    output logic cout
);
// internal connection wire
wire wire0, wire1, wire2, wire3, wire4, wire5, wire6;

    nor_gate g0 ( .a(x),     .b(y),     .y(wire0) );
    nor_gate g1 ( .a(wire0), .b(x),     .y(wire1) );
    nor_gate g2 ( .a(wire0), .b(y),     .y(wire2) );
    nor_gate g3 ( .a(wire1), .b(wire2), .y(wire3) );
    nor_gate g4 ( .a(wire3), .b(cin),   .y(wire4) );
    nor_gate g5 ( .a(wire3), .b(wire4), .y(wire5) );
    nor_gate g6 ( .a(wire4), .b(cin),   .y(wire6) );
    nor_gate g7 ( .a(wire5), .b(wire6), .y(ans)   );
    nor_gate g8 ( .a(wire0), .b(wire4), .y(cout)  );

endmodule

