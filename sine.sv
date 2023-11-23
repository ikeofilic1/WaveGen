`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/17/2023 03:41:08 PM
// Design Name: 
// Module Name: sine
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module SineWaves (
    input CLK,
    input LUT_CLK,
    input EN,
    
    input [31:0] PHASE_A,
    input [31:0] PHASE_B,
    
    output reg signed [15:0] OUTA,
    output reg signed [15:0] OUTB    
);
    localparam LUT_ADDR_WIDTH = 9;    
    wire SIGN_A = PHASE_A[31];
    wire DIR_A = PHASE_A[30];
    wire [LUT_ADDR_WIDTH-1:0] LUTA_index = PHASE_A[29 -: LUT_ADDR_WIDTH];
    
    wire SIGN_B = PHASE_B[31];
    wire DIR_B = PHASE_B[30];
    wire [LUT_ADDR_WIDTH-1:0] LUTB_index = PHASE_B[29 -: LUT_ADDR_WIDTH];
    
    reg [LUT_ADDR_WIDTH-1:0] LUTA_addr;
    reg [LUT_ADDR_WIDTH-1:0] LUTB_addr;    
    wire [15:0] LUTA_value, LUTB_value;
    
    sin_LUT LUT (
      .clka(LUT_CLK),    // input wire clka
      //.ena(EN),      // input wire ena
      .addra(LUTA_addr),  // input wire [8 : 0] addra
      .douta(LUTA_value),  // output wire [15 : 0] douta
      .clkb(LUT_CLK),    // input wire clkb
      //.enb(EN),      // input wire enb
      .addrb(LUTB_addr),  // input wire [8 : 0] addrb
      .doutb(LUTB_value)  // output wire [15 : 0] doutb
    );
    
//    vio_3 sine (
//      .clk(LUT_CLK),              // input wire clk
//      .probe_in0(PHASE_A[31 -: (LUT_ADDR_WIDTH+2)]),  // input wire [31 : 0] probe_in0
//      .probe_in1(LUTA_addr),  // input wire [8 : 0] probe_in1
//      .probe_in2(LUTB_addr),  // input wire [8 : 0] probe_in2
//      .probe_in3(LUTA_value),  // input wire [15 : 0] probe_in3
//      .probe_in4(PHASE_B[31 -: (LUT_ADDR_WIDTH+2)]),  // input wire [31 : 0] probe_in0
//      .probe_in5(LUTB_value)
//    );
    
    always @ (posedge LUT_CLK)
    if (CLK == 1'b1)
        begin
            LUTA_addr = DIR_A ? ~LUTA_index : LUTA_index;
            LUTB_addr = DIR_B ? ~LUTB_index : LUTB_index;
            
            OUTA = SIGN_A ? -LUTA_value : LUTA_value;
            OUTB = SIGN_B ? -LUTB_value : LUTB_value;
        end    
endmodule
