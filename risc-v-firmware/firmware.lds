MEMORY
{
    DDR0 : o = 0x00000000, l = 512M          /* 512Mb external DDR Bank 0 */
	SRAM : o = 0x40000000, l = 0xFFFF  /* 63kB available internal SRAM */
   
}

SECTIONS {
	.memory : {
		. = ALIGN(4);
		*(.init);
		*(.text);
		*(*);		
	} >SRAM 
}

