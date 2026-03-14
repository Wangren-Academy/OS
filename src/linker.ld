/* src/linker.ld */
OUTPUT_FORMAT("binary")
ENTRY(_start)  /* 入口点符号 */

SECTIONS
{
    /* 内核加载地址为 0x10000 */
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
        *(.bss)
    }
    
    /* 确保对齐到扇区边界 */
    . = ALIGN(512);
}