module RAM_1R1W (CK, RCSN, WCSN, RA, WA, D, Q);

  parameter words = 'd16;
  parameter width = 'd16;
  parameter addr_width = 4;

  input CK;
  input RCSN;
  input WCSN;
  input [addr_width-1:0] RA;
  input [addr_width-1:0] WA;
  input [width-1:0] D;
  output reg [width-1:0] Q;

// synopsys translate_off
  reg  [width-1:0] mem [words-1:0];

  integer i;
  always @(posedge CK)
  begin
    if ( RCSN == 0 ) begin
      if ( RA == WA && WCSN == 0 )  // read-write contention
        Q <= {width{1'bX}};
      else
        Q <= mem[RA];
    end
    if ( WCSN == 0 ) begin
      mem[WA] <= D;
    end
  end
// synopsys translate_on

endmodule
