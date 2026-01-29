ENTRY(_start)

SECTIONS
{
	/* Link the kernel at 1MB */
	. = 1M;

	.text : ALIGN(4096)
	{
		KEEP(*(.multiboot))
        KEEP(*(.text))
		*(.text*)
	}

	.rodata : ALIGN(4096)
	{
		*(.rodata*)
        *(.rdata*)
	}

	.data : ALIGN(4096)
	{
		*(.data*)
	}
    
    _edata = .;

	.bss : ALIGN(4096)
	{
		*(.COMMON)
		*(.bss*)
	}
    
    _end = .;
}
