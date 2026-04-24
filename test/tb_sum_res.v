`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 24.09.2024 17:55:50
// Design Name: 
// Module Name: tb_sum_res
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


module tb_sum_res;

 reg [15:0] x1,x2;
 wire [15:0] y;
 reg clk, rst,en;
 wire ready;
 reg add_sub;
 real value1;
 real value2;
   
parameter clk_period = 10;
parameter rst_time = 100;

function [15:0] dtobf16 (input real a);
reg [63:0] a_bits; 
reg s;
reg [7:0]exp;
reg [8:0]mant;
reg [11:0]mant_grs;
reg [6:0] o_mant;
begin
if (a==0.0)
    dtobf16=16'd0;
else begin    
  a_bits=$realtobits(a);
  s=a_bits[63];
  exp=(a_bits[62:52]-1023)+127;

  mant_grs={2'b01,a_bits[51:42]};
  mant=mant_grs[11:3];
  
  if (mant_grs[2:0]>3'd4)
       mant=mant_grs[11:3]+9'd1;
  
  if (mant_grs[2:0]==3'd4) begin
     if(mant_grs[3]==1'b1)
        mant=mant_grs[11:3]+9'd1;
     else
       mant=mant_grs[11:3];
       
   end
       
   if(mant[8]==1'b1) begin
   o_mant=mant[7:1];
    exp=exp+1;
    end
   else
   o_mant=mant[6:0];
         
  
  
  dtobf16={s,exp,o_mant};
  
  
  
  end
  
  
 end

endfunction


//fp16sum_res uut
fp16sum_res_pipe uut
(
      .x1(x1),
      .x2(x2),
      .y(y),
      .clk(clk),
      .rst(rst),
      .add_sub(add_sub),
      .en(en),
      .ready(ready)
);

initial begin

    $dumpfile("test_sumres.vcd");
	$dumpvars(0, tb_sum_res);
    $monitor($time, " clk=%d | x1=%h x2=%h y=%h", clk, x1, x2, y);


    value1=45.83;
    value2=625.41;
    add_sub = 0;
    x1 <= 0;
    x2 <= 0;
    #rst_time;
     
    add_sub = 0;
    x1 <= dtobf16(value1);
    x2 <= dtobf16(value2) ;    
    #(clk_period);
    
    x1 <= dtobf16(-value1);
    x2 <= dtobf16(value2) ;    
    #(clk_period);
    
    x1 <= dtobf16(value1);
    x2 <= dtobf16(-value2) ;
    #(clk_period);
    
    x1 <= dtobf16(-value1);
    x2 <= dtobf16(-value2) ;
    #(clk_period);
    
    add_sub = 1;
    x1 <= dtobf16(value1);
    x2 <= dtobf16(value2) ;    
    #(clk_period);
    
    x1 <= dtobf16(-value1);
    x2 <= dtobf16(value2) ;    
    #(clk_period);
    
    x1 <= dtobf16(value1);
    x2 <= dtobf16(-value2) ;
    #(clk_period);
    
    x1 <= dtobf16(-value1);
    x2 <= dtobf16(-value2) ;
    #(clk_period);

    #(clk_period*20);

    $finish;

   
end


initial begin
    en = 0;
    #rst_time en = 1;
end

initial begin
    clk = 0;
    forever begin
    #(clk_period/2) clk = ~clk; 
    end
end


initial begin
    rst = 1; 
    #rst_time rst = 0;
    
end




endmodule

