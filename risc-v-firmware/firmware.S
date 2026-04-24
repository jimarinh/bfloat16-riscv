.section .init
.global main
.global isr

/* set stack pointer */
lui sp, %hi(0x4000fff0)
addi sp, sp, %lo(0x4000fff0)

/* call main */
jal ra, main

/* break */
ebreak


// Interrupt handler
.org 0x00000010
jal ra, isr

