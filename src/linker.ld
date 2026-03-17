/* linker.ld */
OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x10000;
    
    .text : {
        *(.text)
    }
    
    .data : {
        *(.data)
    }
    
    .rodata : {
        *(.rodata)
    }
    
    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }
}