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
    output IMU_DEN_AG,        // IMU data enable (accel/gyro)
    inout [14:0]DDR_addr,
    inout [2:0]DDR_ba,
    inout DDR_cas_n,
    inout DDR_ck_n,
    inout DDR_ck_p,
    inout DDR_cke,
    inout DDR_cs_n,
    inout [3:0]DDR_dm,
    inout [31:0]DDR_dq,
    inout [3:0]DDR_dqs_n,
    inout [3:0]DDR_dqs_p,
    inout DDR_odt,
    inout DDR_ras_n,
    inout DDR_reset_n,
    inout DDR_we_n,
    inout FIXED_IO_ddr_vrn,
    inout FIXED_IO_ddr_vrp,
    inout [53:0]FIXED_IO_mio,
    inout FIXED_IO_ps_clk,
    inout FIXED_IO_ps_porb,
    inout FIXED_IO_ps_srstb
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
    
    wire signed [15:0] OUT_A, OUT_B;

    wire [11:0] DACA_out, DACB_out;
    assign LED = DACA_out[9:0];
    assign RGB0 = {1'b0, DACA_out[10], 1'b0};
    assign RGB1 = {1'b0, DACA_out[11], 1'b0};
    
     wire SDI, CS, LDAC, SCK;
     assign GPIO = {4'b0, LDAC, SDI, SCK, CS, 16'b0};
    
    // Instantiate DACs
    voltsToDACWords #(.DAC_TWOPOINTFIVE(157), .DAC_ZERO(2077)) DACA(OUT_A, DACA_out);
    voltsToDACWords #(.DAC_TWOPOINTFIVE(146), .DAC_ZERO(2073)) DACB(OUT_B, DACB_out);
    
    DAC_Controller controller(DACA_out, DACB_out, CLK100, CS, SCK, SDI, LDAC);  
    
    system_wrapper system_wrapper_i
    (
        .DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .EN_0(LDAC == 1'b0),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .OUT_A_0(OUT_A),
        .OUT_B_0(OUT_B)
    );
endmodule
