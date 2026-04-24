`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 13.11.2024 19:22:59
// Design Name: 
// Module Name: mux_pcpi
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


module mux_pcpi(
    input [31:0] instr,
    input [15:0] y_div,
    input [15:0] y_mul,
    input [15:0] y_addsub,
    input ready_div,
    input ready_mul,
    input ready_addsub,
    output reg [15:0] y,
    output reg ready,
    output addsub
    );
    
    
    assign addsub=instr[12];
    
    always @(*) begin
    case (instr[13:12])
    2'b00: begin
    ready=ready_mul;
    y=y_mul;
    end
    2'b01: begin
    ready=ready_div;
    y=y_div;
    end
    2'b10: begin
    ready=ready_addsub;  
    y=y_addsub;
    end
    2'b11: begin
    ready=ready_addsub; 
    y=y_addsub; 
    end
    
    endcase

    
    
    end
    
    
    
endmodule
