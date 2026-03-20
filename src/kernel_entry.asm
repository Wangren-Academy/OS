; kernel_entry.asm - 内核入口点（保护模式）
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

    ; 内核不应返回，若返回则死循环
    jmp $