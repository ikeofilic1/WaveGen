`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/26/2023 04:09:45 PM
// Design Name: 
// Module Name: DAC_Controller
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


module DAC_Controller(
    input [11:0] R1,
    input [11:0] R2,
    input CLK100,
    output reg CS,
    output reg SCLK,
    output reg SDI,
    output reg LDAC
);  
    reg SCK;    
    wire four_mega;
    
    DivideByN #( .N(25), .M(5) ) divideby25(.clk(CLK100), .pulse(four_mega));    
    always_ff @ (posedge four_mega)
        SCK = !SCK;

    parameter RESET = 4'd0, CS_LOW1 = 4'd1, CS_HIGH1 = 4'd2, CS_LOW2 = 4'd3, CS_HIGH2 = 4'd4, LDAC_LOW = 4'd5, LDAC_HIGH = 4'd6, NOP = 4'd7;        
    reg [3:0] counter;     
    reg [3:0] state = RESET;
    
    always_ff @ (negedge SCK) begin
        case (state)
            RESET: begin state <= CS_LOW1; counter <= 4'd15; end
            CS_LOW1:
                begin 
                    if (counter == 4'd0) 
                        state <= CS_HIGH1;
                    else counter <= counter - 1; 
                end      
            CS_HIGH1: begin 
                if (counter == 4'd1) begin 
                    state <= CS_LOW2; 
                    counter <= 4'd15; 
                end else counter <= counter + 1; end
            CS_LOW2: begin 
                 if (counter == 4'd0) 
                       state <= CS_HIGH2;
                 else counter <= counter - 1; 
            end
            CS_HIGH2: begin
                if (counter == 4'd1) begin 
                    state <= LDAC_LOW; 
                    counter <= 4'd15; 
                end else counter <= counter + 1; end
            LDAC_LOW: begin state <= LDAC_HIGH; end
            LDAC_HIGH: begin state <= NOP; end
            NOP: begin state <= RESET; end
                        
            default: state <= RESET;
        endcase
    end
    
    // TODO: synchronize REG1 and REG2 with SCK then add to the sensitivity list to shut vivado up
    wire [15:0] REG1 = {4'b0011, R1};
    wire [15:0] REG2 = {4'b1011, R2};    
    always @ (state, counter) begin
        case (state)
            RESET: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
            CS_LOW1: begin CS <= 1'b0; LDAC <= 1'b1; SDI <= REG1[counter]; end //if (counter == 0) counter <= 15; end
            CS_HIGH1: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
            CS_LOW2: begin CS <= 1'b0; LDAC <= 1'b1; SDI <= REG2[counter]; end // if (counter == 0) counter <= 15; end
            CS_HIGH2: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
            LDAC_LOW: begin CS <= 1'b1; LDAC <= 1'b0; SDI <= 1'bz; end
            LDAC_HIGH: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
            NOP: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
            
            default: begin CS <= 1'b1; LDAC <= 1'b1; SDI <= 1'bz; end
        endcase
    end
    
    assign SCLK = SCK;
endmodule