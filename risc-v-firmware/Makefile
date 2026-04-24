TOOLCHAIN_PREFIX = riscv64-unknown-elf-

synth_system: firmware.hex



firmware.hex: firmware.S firmware.c firmware.lds
	$(TOOLCHAIN_PREFIX)gcc -Os -march=rv32i -mabi=ilp32  -ffreestanding -nostdlib -o firmware.elf firmware.S firmware.c \
		 --std=gnu99 -Wl,-Bstatic,-T,firmware.lds,-Map,firmware.map,--strip-debug -lgcc
	$(TOOLCHAIN_PREFIX)objcopy -O binary firmware.elf firmware.bin
	python bin2c.py -i firmware.bin -a rvfirm -o ../vitis/test_bfloat16/src/rvfirm.c



clean:
	rm  firmware.bin firmware.elf firmware.hex firmware.map

