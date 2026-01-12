
module or_gate(
	input logic a,
	input logic b,
	output logic y
);	

logic buf1;

nor_gate(.a(a),.b(b),y(buf1));
not_gate(.a(buf1),.y(y));

endmodule;
