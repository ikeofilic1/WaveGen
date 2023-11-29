`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/08/2023 10:57:20 PM
// Design Name: 
// Module Name: calibration
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

// For A:
// 2.5V = 157 in dac words
// 0V = 2077 

// For B:
// 2.5V = 146 in dac words
// 0V = 2073

module voltsToDACWords
#(
    parameter int DAC_TWOPOINTFIVE = 1, DAC_ZERO = 2048, //ideal conditions, please modify accordingly
    parameter N = 16, M = 12
)(
    input signed [N-1:0] in,
    output [M-1:0] calibrated
);    
    assign calibrated = ($signed(DAC_TWOPOINTFIVE - DAC_ZERO)*in + 12500)/25000 + DAC_ZERO;
endmodule
