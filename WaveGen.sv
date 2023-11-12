`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/08/2023 10:08:11 PM
// Design Name: 
// Module Name: WaveGen
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


module WaveGen(
    input CLK100,           // 100 MHz clock input
    output [9:0] LED,       // RGB1, RGB0, LED 9..0 placed from left to right
    output [2:0] RGB0,      
    output [2:0] RGB1,
    output [3:0] SS_ANODE,   // Anodes 3..0 placed from left to right
    output [7:0] SS_CATHODE, // Bit order: DP, G, F, E, D, C, B, A
    input [11:0] SW,         // SWs 11..0 placed from left to right
    input [3:0] PB,          // PBs 3..0 placed from left to right
    inout [23:0] GPIO,       // PMODA-C 1P, 1N, ... 3P, 3N order
    output [3:0] SERVO,      // Servo outputs
    output PDM_SPEAKER,      // PDM signals for mic and speaker
    input PDM_MIC_DATA,      
    output PDM_MIC_CLK,
    output ESP32_UART1_TXD,  // WiFi/Bluetooth serial interface 1
    input ESP32_UART1_RXD,
    output IMU_SCLK,         // IMU spi clk
    output IMU_SDI,          // IMU spi data input
    input IMU_SDO_AG,        // IMU spi data output (accel/gyro)
    input IMU_SDO_M,         // IMU spi data output (mag)
    output IMU_CS_AG,        // IMU cs (accel/gyro) 
    output IMU_CS_M,         // IMU cs (mag)
    input IMU_DRDY_M,        // IMU data ready (mag)
    input IMU_INT1_AG,       // IMU interrupt (accel/gyro)
    input IMU_INT_M,         // IMU interrupt (mag)
    output IMU_DEN_AG        // IMU data enable (accel/gyro)
);
    // Terminate all of the unused outputs or i/o's
    // assign LED = 10'b0000000000;
    // assign RGB0 = 3'b000;
    // assign RGB1 = 3'b000;
    assign SS_ANODE = 4'b0000;
    assign SS_CATHODE = 8'b11111111;
    assign SERVO = 4'b0000;
    assign PDM_SPEAKER = 1'b0;
    assign PDM_MIC_CLK = 1'b0;
    assign ESP32_UART1_TXD = 1'b0;
    assign IMU_SCLK = 1'b0;
    assign IMU_SDI = 1'b0;
    assign IMU_CS_AG = 1'b1;
    assign IMU_CS_M = 1'b1;
    assign IMU_DEN_AG = 1'b0;
    
    // For A:
    // 2.5V = 157 in dac words
    // 0V = 2077 
    
    // For B:
    // 2.5V = 146 in dac words
    // 0V = 2073

    wire [11:0] DACA_out, DACB_out;
    assign LED = DACA_out[9:0];
    assign RGB0 = {1'b0, DACA_out[10], 1'b0};
    assign RGB1 = {1'b0, DACA_out[11], 1'b0};
    
    reg signed [15:0] input_val;    
    always_comb
    case (SW[1:0])
        2'b10: input_val = -25000;
        2'b11: input_val = -25000;
        2'b00: input_val = 0;
        2'b01: input_val = 25000;        
    endcase
    
     wire SDI, CS, LDAC, SCK;
     assign GPIO = {4'b0, LDAC, SDI, SCK, CS, 16'b0};
    
    // Instantiate DACs
    voltsToDACWords #(.DAC_TWOPOINTFIVE(157), .DAC_ZERO(2077)) DACA(input_val, DACA_out);
    voltsToDACWords #(.DAC_TWOPOINTFIVE(146), .DAC_ZERO(2073)) DACB(input_val, DACB_out);
    
    DAC_Controller(DACA_out, DACB_out, CLK100, CS, SCK, SDI, LDAC);    
endmodule
