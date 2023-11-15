`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/14/2023 07:49:09 PM
// Design Name: 
// Module Name: Waveforms
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
    input EN,
    
    input [31:0] PHASE_A,
    input [31:0] PHASE_B,
    
    output signed [15:0] OUTA,
    output signed [15:0] OUTB    
);
    localparam LUT_ADDR_WIDTH = 9;    
    wire SIGN_A = PHASE_A[31];
    wire DIR_A = PHASE_A[30];
    wire [LUT_ADDR_WIDTH-1:0] LUTA_index = PHASE_A[29 -: LUT_ADDR_WIDTH];
    
    wire SIGN_B = PHASE_B[31];
    wire DIR_B = PHASE_B[30];
    wire [LUT_ADDR_WIDTH-1:0] LUTB_index = PHASE_B[29 -: LUT_ADDR_WIDTH];
    
    wire [LUT_ADDR_WIDTH-1:0] LUTA_addr;
    wire [LUT_ADDR_WIDTH-1:0] LUTB_addr;    
    wire signed [15:0] LUTA_value, LUTB_value;
    
    sin_LUT LUT (
      .clka(CLK),    // input wire clka
      .ena(EN),      // input wire ena
      .addra(LUTA_addr),  // input wire [8 : 0] addra
      .douta(LUTA_value),  // output wire [15 : 0] douta
      .clkb(CLK),    // input wire clkb
      .enb(EN),      // input wire enb
      .addrb(LUTB_addr),  // input wire [8 : 0] addrb
      .doutb(LUTB_value)  // output wire [15 : 0] doutb
    );
    
    assign LUTA_addr = DIR_A ? ~LUTA_index : LUTA_index;
    assign LUTB_addr = DIR_B ? ~LUTB_index : LUTB_index;
    
    assign OUTA = SIGN_A ? -LUTA_value : LUTA_value;
    assign OUTB = SIGN_B ? -LUTB_value : LUTB_value;
endmodule

module SquareWave (
    input CLK,
    
    input [31:0] DTCYC,
    input [31:0] OFFSET,
    input [31:0] DELTA,
    
    output reg [15:0] OUT
);

    localparam ONE_VOLT = 2**15;
    localparam ZERO_VOLT = 0;
    
    reg [31:0] counter = OFFSET;
    always @ (posedge CLK)
        begin
            if (counter >= DTCYC)
                OUT = ZERO_VOLT;
            else
                OUT = ONE_VOLT;
                
            counter += DELTA;
        end    
endmodule

module WaveForms 
#(
    parameter int SAMPLING_FREQUENCY = 500000000
)(
    input CLK,
    input LUT_CLK,
    input RST,
    
    input [3:0] MODE_A,
    input [3:0] MODE_B,
    input [31:0] FREQ_A,
    input [31:0] FREQ_B,
    input [15:0] DTCYC_A,
    input [15:0] DTCYC_B,
    input signed [15:0] PHASE_OFFS_A,
    input signed [15:0] PHASE_OFFS_B,
    input [15:0] CYCLES_A,
    input [15:0] CYCLES_B,
    
    output reg signed [15:0] WAVE_A,
    output reg signed [15:0] WAVE_B
);
    localparam DC = 4'd0, SINE = 4'd1, SAWTOOTH = 4'd2, TRIANGLE = 4'd3, SQUARE = 4'd4;
    
    reg [31:0] phase_a = 0;
    integer unsigned delta_phase_a = FREQ_A*(2**32)/SAMPLING_FREQUENCY;
    wire signed [31:0] normalized_phase_offset_a = $signed(PHASE_OFFS_A*(2**32)/36000); // phase offset is from -180 degrees to 180 degrees
    wire [31:0] real_phase_a = phase_a + normalized_phase_offset_a; 
    
         
   reg [31:0] phase_b = 0;
   integer unsigned delta_phase_b = FREQ_B*(2**32)/SAMPLING_FREQUENCY;
   wire signed [31:0] normalized_phase_offset_b = $signed(PHASE_OFFS_B*(2**32)/36000); // phase offset is from -180 degrees to 180 degrees
   wire [31:0] real_phase_b = phase_b + normalized_phase_offset_b;
    
    wire signed [15:0] sine_a, sine_b;
    SineWaves(LUT_CLK, 1'b1, real_phase_a, real_phase_b, sine_a, sine_b);
      
    wire [15:0] square_a;
    SquareWave squareA(
        CLK, DTCYC_A << 16, normalized_phase_offset_a, 
        delta_phase_a, square_a
    ); 
      
    wire [15:0] square_b;
    SquareWave squareB(
        CLK, DTCYC_B << 16, normalized_phase_offset_b, 
        delta_phase_b, square_b
    );   
    
    reg [15:0] n_cycles_a = 0;
    always @ (negedge RST or negedge phase_a[31])
        if (RST == 1'b0)
            n_cycles_a <= 0;
        else if (n_cycles_a != CYCLES_A)
            n_cycles_a <= n_cycles_a + 1;          
    
    always @ (posedge CLK or negedge RST)
        if (RST == 1'b0)
        begin
            phase_a <= 32'b0;
            WAVE_A <= 16'b0;
        end
        else if (CYCLES_A == 0 || n_cycles_a != CYCLES_A)     
        begin
            case (MODE_A)
                DC:
                    WAVE_A <= 0;
                SINE:
                    begin WAVE_A <= sine_a; end
                SAWTOOTH:
                    if (real_phase_a >= 0 && real_phase_a < 2**31)
                        //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**16 - 2**15;
                        WAVE_A <= real_phase_a/2**16; // 2x
                    else
                        WAVE_A <= real_phase_a/2**16 - 2**16; // 2x - 2                 
                TRIANGLE:
                begin                
                //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**15 - 2**16;
                
                    if (real_phase_a >= 0 && real_phase_a < 2**30)
                        WAVE_A <= real_phase_a/2**15; // 4x
                    else if (real_phase_a >= (2**30) && real_phase_a < 3*(2**30))
                        WAVE_A <= 2**16 - real_phase_a/2**15; // 2 - 4x
                    else // real_phase >= 3*(2**30)
                        WAVE_A <= real_phase_a/2**15 - 2**17; // 4x - 4
                end
                SQUARE:
                    WAVE_A <= square_a;
            endcase
            phase_a <= phase_a + delta_phase_a;
        end
        else
            WAVE_A <= 16'b0;

    reg [15:0] n_cycles_b = 0;        
    always @ (negedge RST or negedge phase_b[31])
        if (RST == 1'b0)
            n_cycles_b <= 0;
        else if (n_cycles_b != CYCLES_B)
            n_cycles_b <= n_cycles_b + 1;          
    
    always @ (posedge CLK or negedge RST)
    if (RST == 1'b0)
    begin
        phase_b <= 32'b0;
        WAVE_B <= 16'b0;
    end
    else if (CYCLES_B == 0 || n_cycles_b != CYCLES_B)     
    begin
        case (MODE_B)
            DC:
                WAVE_B <= 0;
            SINE:
                begin WAVE_B <= sine_b; end
            SAWTOOTH:
                if (real_phase_b >= 0 && real_phase_b < 2**31)
                    //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**16 - 2**15;
                    WAVE_B <= real_phase_b/2**16; // 2x
                else
                    WAVE_B <= real_phase_b/2**16 - 2**16; // 2x - 2                 
            TRIANGLE:
            begin                
            //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**15 - 2**16;
            
                if (real_phase_b >= 0 && real_phase_b < 2**30)
                    WAVE_B <= real_phase_b/2**15; // 4x
                else if (real_phase_a >= (2**30) && real_phase_b < 3*(2**30))
                    WAVE_B <= 2**16 - real_phase_b/2**15; // 2 - 4x
                else // real_phase >= 3*(2**30)
                    WAVE_B <= real_phase_b/2**15 - 2**17; // 4x - 4
            end
            SQUARE:
                WAVE_B <= square_b;
        endcase
        phase_b <= phase_b + delta_phase_b;
    end
    else
        WAVE_B <= 16'b0;
            
endmodule