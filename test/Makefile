SHELL = bash
PYTHON = python3
ICARUS_SUFFIX =
IVERILOG = iverilog$(ICARUS_SUFFIX)
VVP = vvp$(ICARUS_SUFFIX)

VERILOG_OBJS =  ../src/fp16_sum_res_pipe.v ../src/fpmul.v ../src/fpdiv.v \
				../src/sum_res_pp.v ../src/add_renorm.v ../src/exp_mant_logic.v \
				../src/fractional_divider.v ../src/leading_zero_norm.v \
				../src/myreg.v ../src/op_sign_logic.v ../src/rounder.v
				
all: test_sumres test_mul

test_sumres: test_sumres.vvp
	$(VVP) -N $< +vcd

test_sumres.vvp: $(VERILOG_OBJS) tb_sum_res.v
	$(IVERILOG) -o $@ $^

test_mul: test_mul.vvp
	$(VVP) -N $< +vcd

test_mul.vvp: $(VERILOG_OBJS) tb_mul.v
	$(IVERILOG) -o $@ $^

clean:
	rm -vrf *.vvp *.vcd
