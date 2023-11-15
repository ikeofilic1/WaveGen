`timescale 1 ns / 1 ps

module wavegen_v1_0_S00_AXI #
(
    // Bit width of S_AXI address bus
    parameter integer C_S_AXI_ADDR_WIDTH = 6,
    parameter integer SAMPLING_FREQUENCY = 500000000
)
(
    // Ports to top level module (what makes this the Wavegen IP module)
    input sample_clk,
    input LUT_CLK,
    output signed [15:0] OUT_A,
    output signed [15:0] OUT_B,
    
    // AXI clock and reset        
    input wire S_AXI_ACLK,
    input wire S_AXI_ARESETN,

    // AXI write channel
    // address:  add, protection, valid, ready
    // data:     data, byte enable strobes, valid, ready
    // response: response, valid, ready 
    input wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_AWADDR,
    input wire [2:0] S_AXI_AWPROT,
    input wire S_AXI_AWVALID,
    output wire S_AXI_AWREADY,
    
    input wire [31:0] S_AXI_WDATA,
    input wire [3:0] S_AXI_WSTRB,
    input wire S_AXI_WVALID,
    output wire  S_AXI_WREADY,
    
    output wire [1:0] S_AXI_BRESP,
    output wire S_AXI_BVALID,
    input wire S_AXI_BREADY,
    
    // AXI read channel
    // address: add, protection, valid, ready
    // data:    data, resp, valid, ready
    input wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_ARADDR,
    input wire [2:0] S_AXI_ARPROT,
    input wire S_AXI_ARVALID,
    output wire S_AXI_ARREADY,
    
    output wire [31:0] S_AXI_RDATA,
    output wire [1:0] S_AXI_RRESP,
    output wire S_AXI_RVALID,
    input wire S_AXI_RREADY
);

    // Internal registers
    reg [2:0] mode_a, mode_b;
    reg enable_a, enable_b; //used
    reg [31:0] freq_a;
    reg [31:0] freq_b;
    reg [15:0] offset_a, offset_b; //used
    reg [15:0] amp_a, amp_b; //used
    reg [15:0] dtcyc_a, dtcyc_b;
    reg [15:0] cycles_a, cycles_b;
    reg [15:0] phase_off_a, phase_off_b;
    
    wire signed [15:0] wave_a_value; //used
    wire signed [15:0] wave_b_value; //used
   
    assign OUT_A = enable_a ? wave_a_value*amp_a/(2**15) + offset_a : 16'd0;
    assign OUT_B = enable_b ? wave_b_value*amp_b/(2**15) + offset_b : 16'd0;
    
    // Wave instantiations  
    WaveForms # (
        .SAMPLING_FREQUENCY(SAMPLING_FREQUENCY)
    ) A(
        sample_clk, LUT_CLK, 1'b1, 
        mode_a, mode_b, freq_a, freq_b, dtcyc_a, dtcyc_b, 
        phase_off_a, phase_off_b, cycles_a, cycles_b,
        wave_a_value, wave_b_value
    );
    
    // Register map
    // ofs  fn
    //   0  mode (r/w)   -
    //   4  run (r/w)    -
    //   8  freqA (r/w) units of 100uHz
    //  12  freqB (r/w) units of 100uHz
    //  16  offset (r/w) units of 100uV
    //  20  ampltd (r/w) units of 100uV
    //  24  dtcyc (r/w) units of 100%/2**16
    //  28  cycles (r/w) units of 1 cycle
    //  32  phase_off (r/w) units of 0.01 degrees (-180 to 180)
    
    // Register numbers
    localparam integer MODE_REG       = 4'b0000;
    localparam integer RUN_REG        = 4'b0001;
    localparam integer FREQ_A_REG     = 4'b0010;
    localparam integer FREQ_B_REG     = 4'b0011;
    localparam integer OFFSET_REG     = 4'b0100;
    localparam integer AMPLTD_REG     = 4'b0101;
    localparam integer DTCYC_REG      = 4'b0110;
    localparam integer CYCLES_REG     = 4'b0111;
    localparam integer PHASE_OFF_REG  = 4'b1000;
    
    // AXI4-lite signals
    reg axi_awready;
    reg axi_wready;
    reg [1:0] axi_bresp;
    reg axi_bvalid;
    reg axi_arready;
    reg [31:0] axi_rdata;
    reg [1:0] axi_rresp;
    reg axi_rvalid;
    
    // friendly clock, reset, and bus signals from master
    wire axi_clk           = S_AXI_ACLK;
    wire axi_resetn        = S_AXI_ARESETN;
    wire [31:0] axi_awaddr = S_AXI_AWADDR;
    wire axi_awvalid       = S_AXI_AWVALID;
    wire axi_wvalid        = S_AXI_WVALID;
    wire [3:0] axi_wstrb   = S_AXI_WSTRB;
    wire axi_bready        = S_AXI_BREADY;
    wire [31:0] axi_araddr = S_AXI_ARADDR;
    wire axi_arvalid       = S_AXI_ARVALID;
    wire axi_rready        = S_AXI_RREADY;    
    
    // assign bus signals to master to internal reg names
    assign S_AXI_AWREADY = axi_awready;
    assign S_AXI_WREADY  = axi_wready;
    assign S_AXI_BRESP   = axi_bresp;
    assign S_AXI_BVALID  = axi_bvalid;
    assign S_AXI_ARREADY = axi_arready;
    assign S_AXI_RDATA   = axi_rdata;
    assign S_AXI_RRESP   = axi_rresp;
    assign S_AXI_RVALID  = axi_rvalid;
  
    // Assert address ready handshake (axi_awready) 
    // - after address is valid (axi_awvalid)
    // - after data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    // De-assert ready (axi_awready)
    // - after write response channel ready handshake received (axi_bready)
    // - after this module sends write response channel valid (axi_bvalid) 
    wire wr_add_data_valid = axi_awvalid && axi_wvalid;
    reg aw_en;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_awready <= 1'b0;
            aw_en <= 1'b1;
        end
        else  
        begin
            if (wr_add_data_valid && ~axi_awready && aw_en)
            begin
                axi_awready <= 1'b1;
                aw_en <= 1'b0;
            end
            else if (axi_bready && axi_bvalid)
                begin
                    aw_en <= 1'b1;
                    axi_awready <= 1'b0;
                end
            else           
                axi_awready <= 1'b0;
        end 
    end

    // Capture the write address (axi_awaddr) in the first clock (~axi_awready)
    // - after write address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    reg [C_S_AXI_ADDR_WIDTH-1:0] waddr;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            waddr <= 0;
        else if (wr_add_data_valid && ~axi_awready && aw_en)
            waddr <= axi_awaddr;
    end

    // Output write data ready handshake (axi_wready) generation for one clock
    // - after address is valid (axi_awvalid)
    // - after data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            axi_wready <= 1'b0;
        else
            axi_wready <= (wr_add_data_valid && ~axi_wready && aw_en);
    end       

    // Write data to internal registers
    // - after address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - after this module asserts ready for address handshake (axi_awready)
    // - after this module asserts ready for data handshake (axi_wready)
    // write correct bytes in 32-bit word based on byte enables (axi_wstrb)
    // int_clear_request write is only active for one clock
    wire wr = wr_add_data_valid && axi_awready && axi_wready;
    integer byte_index;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            mode_a <= 3'b0;
            mode_b <= 3'b0;
            enable_a <= 1'b1;
            enable_b <= 1'b1;
            freq_a <= 32'b0;
            freq_b <= 32'b0;
            offset_a <= 16'b0;
            offset_b <= 16'b0;
            amp_a <= 16'b0;
            amp_b <= 16'b0;
            dtcyc_a <= 16'b0;
            dtcyc_b <= 16'b0;
            cycles_a <= 16'b0;
            cycles_b <= 16'b0;
            phase_off_a <= 16'b0;
            phase_off_b <= 16'b0;
        end 
        else 
        begin
            if (wr)
            begin
                case (waddr[5:2])
                    MODE_REG:
                        if (axi_wstrb[0] == 1)
                            {mode_b, mode_a} <= S_AXI_WDATA[5:0];
                    RUN_REG:
                       if (axi_wstrb[0] == 1)
                            {enable_b, enable_a} <= S_AXI_WDATA[1:0];
                    FREQ_A_REG: 
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                freq_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    FREQ_B_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                freq_b[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    OFFSET_REG:
                    begin
                        for (byte_index = 0; byte_index <= 1; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                offset_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                                
                        for (byte_index = 2; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                offset_b[((byte_index-2)*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    end
                    AMPLTD_REG:
                    begin
                        for (byte_index = 0; byte_index <= 1; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                amp_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                                
                        for (byte_index = 2; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                amp_b[((byte_index-2)*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    end
                    DTCYC_REG:
                    begin
                        for (byte_index = 0; byte_index <= 1; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                dtcyc_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                                
                        for (byte_index = 2; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                dtcyc_b[((byte_index-2)*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    end
                    CYCLES_REG:
                    begin
                        for (byte_index = 0; byte_index <= 1; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                cycles_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                                
                        for (byte_index = 2; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                cycles_b[((byte_index-2)*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    end                    
                    PHASE_OFF_REG:
                    begin
                        for (byte_index = 0; byte_index <= 1; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                phase_off_a[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                                
                        for (byte_index = 2; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                phase_off_b[((byte_index-2)*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    end
                endcase
            end
        end
    end    

    // Send write response (axi_bvalid, axi_bresp)
    // - after address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - after this module asserts ready for address handshake (axi_awready)
    // - after this module asserts ready for data handshake (axi_wready)
    // Clear write response valid (axi_bvalid) after one clock
    wire wr_add_data_ready = axi_awready && axi_wready;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_bvalid  <= 0;
            axi_bresp   <= 2'b0;
        end 
        else
        begin
            if (wr_add_data_valid && wr_add_data_ready && ~axi_bvalid)
            begin
                axi_bvalid <= 1'b1;
                axi_bresp  <= 2'b0;
            end
            else if (S_AXI_BREADY && axi_bvalid) 
                axi_bvalid <= 1'b0; 
        end
    end   

    // In the first clock (~axi_arready) that the read address is valid
    // - capture the address (axi_araddr)
    // - output ready (axi_arready) for one clock
    reg [C_S_AXI_ADDR_WIDTH-1:0] raddr;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_arready <= 1'b0;
            raddr <= 32'b0;
        end 
        else
        begin    
            // if valid, pulse ready (axi_rready) for one clock and save address
            if (axi_arvalid && ~axi_arready)
            begin
                axi_arready <= 1'b1;
                raddr  <= axi_araddr;
            end
            else
                axi_arready <= 1'b0;
        end 
    end       
        
    // Update register read data
    // - after this module receives a valid address (axi_arvalid)
    // - after this module asserts ready for address handshake (axi_arready)
    // - before the module asserts the data is valid (~axi_rvalid)
    //   (don't change the data while asserting read data is valid)
    wire rd = axi_arvalid && axi_arready && ~axi_rvalid;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_rdata <= 32'b0;
        end 
        else
        begin    
            if (rd)
            begin
		// Address decoding for reading registers
		case (raddr[5:2])
		    MODE_REG: 
		        axi_rdata <= {mode_b, mode_a};
		    RUN_REG:
		        axi_rdata <= {enable_b, enable_a};
		    FREQ_A_REG: 
		        axi_rdata <= freq_a;
		    FREQ_B_REG: 
			    axi_rdata <= freq_b;
		    OFFSET_REG:
			    axi_rdata <= {offset_b, offset_a};
		    AMPLTD_REG:
			    axi_rdata <= {amp_b, amp_a};
		    DTCYC_REG:
			    axi_rdata <= {cycles_b, offset_a};
		    CYCLES_REG:
		        axi_rdata <= {cycles_b, cycles_a};
		    PHASE_OFF_REG:
		        axi_rdata <= {phase_off_b, phase_off_a};
		endcase
            end   
        end
    end    

    // Assert data is valid for reading (axi_rvalid)
    // - after address is valid (axi_arvalid)
    // - after this module asserts ready for address handshake (axi_arready)
    // De-assert data valid (axi_rvalid) 
    // - after master ready handshake is received (axi_rready)
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            axi_rvalid <= 1'b0;
        else
        begin
            if (axi_arvalid && axi_arready && ~axi_rvalid)
            begin
                axi_rvalid <= 1'b1;
                axi_rresp <= 2'b0;
            end   
            else if (axi_rvalid && axi_rready)
                axi_rvalid <= 1'b0;
        end
    end    
endmodule