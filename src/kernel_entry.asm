; kernel_entry.asm
[bits 32]
[extern kernel_main]
[global _start]

[extern _bss_start]
[extern _bss_end]

_start:
    ; 清零 BSS 段
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb

    ; 调用内核主函数
    call kernel_main
    jmp $