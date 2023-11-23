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

module WaveForms 
#(
    parameter int SAMPLING_FREQUENCY = 50000
)(
    input CLK,
    input LUT_CLK,
    input ENA,
    input ENB,
    
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
    localparam ONE_VOLT = 2**15 - 1;
    
    reg  [31:0] phase_a = 0;
    wire [31:0] delta_phase_a = ({FREQ_A, 32'b0})/SAMPLING_FREQUENCY;
    
    wire signed [31:0] normalized_phase_offset_a = ({PHASE_OFFS_A, 30'b0})/9000; // phase offset is from -180 degrees to 180 degrees
    wire [31:0] real_phase_a = phase_a + normalized_phase_offset_a; 
         
    reg [31:0] phase_b = 0;
    wire [31:0] delta_phase_b = ({FREQ_B, 32'b0})/SAMPLING_FREQUENCY;
      
    wire signed [31:0] normalized_phase_offset_b = ({PHASE_OFFS_B, 30'b0})/9000; // phase offset is from -180 degrees to 180 degrees
    wire [31:0] real_phase_b = phase_b + normalized_phase_offset_b;
    
    wire signed [15:0] sine_a, sine_b;
    SineWaves(CLK, LUT_CLK, 1'b1, real_phase_a, real_phase_b, sine_a, sine_b);
    
    wire [31:0] dtcyca = {DTCYC_A, 16'b0}; 
    wire [31:0] dtcycb = {DTCYC_B, 16'b0}; 
    
//    vio_2 square(
//        .clk(LUT_CLK),
//        .probe_in0(normalized_phase_offset_a),
//        .probe_in1(delta_phase_a),
//        .probe_in2(delta_phase_b),
//        .probe_in3(real_phase_a),
//        .probe_in4(normalized_phase_offset_b),
//        .probe_in5(real_phase_b),
//        .probe_in6({WAVE_A, WAVE_B})
//   ); 
      
    reg [15:0] n_cycles_a = 0;
    always @ (negedge phase_a[31] or negedge ENA)
        if (ENA == 1'b0)
            n_cycles_a <= 0;
        else if (n_cycles_a != CYCLES_A)
            n_cycles_a <= n_cycles_a + 1;
  
    always @ (posedge CLK)
        if (ENA == 1'b0)
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
                        WAVE_A <= real_phase_a/2**16 - (2**16-1); // 2x - 2                 
                TRIANGLE:
                begin                
                //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**15 - 2**16;
                
                    if (real_phase_a >= 0 && real_phase_a < 2**30)
                        WAVE_A <= real_phase_a/2**15; // 4x
                    else if (real_phase_a >= (2**30) && real_phase_a < 3*(2**30))
                        WAVE_A <= (2**16-1) - real_phase_a/2**15; // 2 - 4x
                    else // real_phase >= 3*(2**30)
                        WAVE_A <= real_phase_a/2**15 - (2**17-2); // 4x - 4
                end
                SQUARE:
                    if (real_phase_a >= dtcyca)
                        WAVE_A <= -ONE_VOLT;
                    else 
                        WAVE_A <= ONE_VOLT;
            endcase
            phase_a <= phase_a + delta_phase_a;
        end
        else
            WAVE_A <= 16'b0;

    reg [15:0] n_cycles_b = 0;        
    always @ (negedge phase_b[31])
        if (ENB == 1'b0)
            n_cycles_b <= 0;
        else if (n_cycles_b != CYCLES_B)
            n_cycles_b <= n_cycles_b + 1;          

    always @ (posedge CLK)
    if (ENB == 1'b0)
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
                    WAVE_B <= real_phase_b/2**16 - (2**16-1); // 2x - 2                 
            TRIANGLE:
            begin                
            //WAVE <= ((real_phase + 2**31) & 32'hFFFFFFFF)/2**15 - 2**16;
            
                if (real_phase_b >= 0 && real_phase_b < 2**30)
                    WAVE_B <= real_phase_b/2**15; // 4x
                else if (real_phase_b >= (2**30) && real_phase_b < 3*(2**30))
                    WAVE_B <= (2**16-1) - real_phase_b/2**15; // 2 - 4x
                else // real_phase >= 3*(2**30)
                    WAVE_B <= real_phase_b/2**15 - (2**17-2); // 4x - 4
            end
            SQUARE:
                if (real_phase_b >= dtcycb)
                    WAVE_B <= -ONE_VOLT;
                else 
                    WAVE_B <= ONE_VOLT;
        endcase
        phase_b <= phase_b + delta_phase_b;
    end
    else
        WAVE_B <= 16'b0;
            
endmodule