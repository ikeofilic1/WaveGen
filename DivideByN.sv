`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/21/2023 06:21:44 PM
// Design Name: 
// Module Name: DivideByN
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


module DivideByN
    #(  parameter N = 100,
        parameter M = 7 
    )
(
    input clk,
    output reg pulse
);

    reg [M-1:0] counter = 0;
    
    always @ (posedge clk)
        if (counter == N-1) 
        begin
            counter = 0;
            pulse = 1'b1;
        end
        else begin counter += 1; pulse = 1'b0; end
endmodule
