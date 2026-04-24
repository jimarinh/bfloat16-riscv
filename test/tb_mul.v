`timescale 1ns / 1ps


// Module for multiplier testbench
module tb_mul;

// Multiplier signals
reg clk,en,rst;
reg [15:0] x1,x2;
wire [15:0] y;
wire ready;

// clk_period in timescale units
parameter clk_period=10;
// FPGA reset time for timing simulation
parameter rst_time=100;

// Unit under test
fpmul uut(
.clk(clk),
.en(en),
.rst(rst),
.ready(ready),
.x1(x1),
.x2(x2),
.y(y)
);

// Procedural block to generate input signals
initial begin

    $dumpfile("test_mul.vcd");
	$dumpvars(0, tb_mul);
    $monitor($time, " clk=%d | x1=%h x2=%h y=%h", clk, x1, x2, y);


    // Test half-precision numbers
    x1<=0;
    x2<=0;
    #rst_time
    x1<=16'hB41B; // -0.2566
    x2=16'h622A; // 789
    #(clk_period)
    x1<=16'h622A; // 789
    x2=16'h622A; // 789
    #(clk_period) 
    x1<=16'h0000; // 0
    x2=16'h0000; // 0
    #(clk_period)
    x1<=16'h8400; // -0.00006104
    x2=16'h0400; // 0.00006104
    #(clk_period)
    x1<=16'h5c00; // 256
    x2=16'hdc00; // -256
    #(clk_period)
    x1<=16'h4248; // 3.141
    x2=16'h416f; // 2.717
    #(clk_period)
    x1<=16'h1fff; // 0.00781
    x2=16'h23ff; //0.01562
    $finish;
end

// Procedural block to generate clock signal
initial begin
    clk = 0;
    forever begin        
        #(clk_period/2) clk = ~clk;
    end
end

// Procedural block to generate reset signal
initial begin
    rst = 1;
    #rst_time rst=0;
end


// Procedural block to generate enable signal
initial begin
    en = 0;
    #rst_time en=1;
    #(clk_period) en = 0;
end


endmodule
