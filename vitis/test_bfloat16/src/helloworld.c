/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "rvfirm.h"



// Write RISC-V firmware to BRAM
void write_rvfirm(char *firm, int n) {

	// Pointer to reset GPIO
	volatile char *rst_gpio = (volatile char *) 0x81200000;

	// Pointer to BRAM
	volatile char *bram = (volatile char *) 0x40000000;

	// Reset RISC-V CPU
	*rst_gpio = 0;

	// Write firmware to BRAM
	//for (int i = 0; i < n; i++) {
		//bram[i] = firm[i];
	//}

	// Flush cache
	Xil_DCacheFlushRange(bram, n);

	// Start RISC-V CPU
	*rst_gpio = 1;
}

// Convert bfloat16 to float
float bftof(uint16_t x)
{

    uint32_t aux = (uint32_t)x << 16;

    return *((float *)&aux);
}



int main() {
	char counter = 0;
    uint16_t y_mul,y_div,y_add,y_sub;

	init_platform();

	print("RISC-V coprocessor test\n\r");

	// Write RISC-V firmware to BRAM
	write_rvfirm(rvfirm, N_RVFIRM);

	// Endless loop reading shared memory region
	while (1) {

		// Invalidate cache
		Xil_DCacheInvalidateRange(0x10000000, 16);

		// Print shared memory contents
		y_mul=*(volatile int*) 0x10000000;
        y_div=*(volatile int*) 0x10000004;
        y_add=*(volatile int*) 0x10000008;
        y_sub=*(volatile int*) 0x1000000c;

        printf("0 xor 0: %f\n",bftof(y_mul));
        printf("0 xor 1: %f\n",bftof(y_div));
        printf("1 xor 0: %f\n",bftof(y_add));
        printf("1 xor 1: %f\n",bftof(y_sub));

		// Sleep for 1 second
		sleep(1);
	}

	cleanup_platform();
	return 0;
}
