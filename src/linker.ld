/* linker.ld - 内核链接脚本 */
OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x10000;          /* 内核加载地址 */

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