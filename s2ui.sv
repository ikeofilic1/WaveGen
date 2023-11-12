`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/08/2023 06:17:11 PM
// Design Name: 
// Module Name: s2ui
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


module s2ui(
    input signed [11:0] signed_in,
    output [11:0] unsigned_out
);
    
    assign unsigned_out = -1*signed_in + 2048;
endmodule
