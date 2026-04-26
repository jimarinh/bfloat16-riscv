# Bfloat extensions for RISC-V

## Description

This repository presents a neural accelerator based on a RISC-V microprocessor whose Instruction Set Architecture (ISA) has been extended with brain floating point (bfloat16) instructions.  

We added four new instructions for addition, subtraction, multiplication, and division which were implemented through circuits described in Verilog; each module includes nearest-even rounding circuits to reduce accumulation of errors.

This repository includes unitary tests (located under ``test`` folder) for behavioral testing on IcarusVerilog, and a validation on FPGA using a multiprocessor-based system on an ARM+FPGA System-on-Chip (SoC) using Xilinx Vitis (folder ``vitis``).

Cite this as 
> López-Parrado, A., Capacho-Valbuena, L.M., Vera-Tasama, A., and Marin-Hurtado, J.I., "Neural Accelerator based on RISC-V with BFloat16 Extensions." in IBERCHIP 2026, Arequipa, Peru, 2026.

## Folder structure

The folder structure is as follows:

* ``src``: Verilog files for bfloat16-arithmetic modules: MULT, DIV, ADD/SUBS

* ``test``: Unitary test for individual modules using IcarusVerilog. To run all tests type ``make`` or for individual test, type for example ``make test_sumres``. These tests generate VCD files compatible with tools such as GTKWave.

* ``risc-v-firmware``:  RISC-V firmware for using the bfloat16 extensions. This firmware generates the library for the implementation on the Xilinx Zynq FPGA.  

* ``vitis``: Xilinx Vivado project for testing the bfloat16 extensions on a Vitis SoC. The SoC uses an ARM side has a role of manager, and the RISC-V microprocessor with bfloat16 extensions is the neural coprocessor. Both microprocessors behave as masters, so that they can communicate with each other by using shared memory.

## Extension's Performance

| Instruction	 | Cycles | Architecture |
|------|------|------|
| bf16add / bf16sub	| 4 | Pipeline |
| bf16mul |3 | Pipeline |
| bf16div | 14 | Serial division |

ADD/SUB and MUL uses a pipeline implementation to provide an one-cycle throughput. DIV uses a serial division for lower hardware complexity. 

## SoC Testing

Tests on Xilinx Zynq uses the hardwired ARM to fetch data into shared memory with a RISC-V-based  microprocessor (PicoRV32 core) with PCPI extensions for bfloat16 arithmetic. The test of the neuronal accelerator was a Multilayer Perceptron (MLP) that solves the exclusive or (XOR) problem. This test showed that our RISC-V-based neural accelerator with bfloat16 extensions speeds up the MLP computation 8x compared to the RISC-V microprocessor without bfloat16 extensions.