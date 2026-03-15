; src/kernel_entry.asm
[bits 32]
[extern kernel_main]
[global _start]

; 导入链接脚本定义的符号
[extern _bss_start]
[extern _bss_end]

_start:
    ; 清零 BSS 段
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi          ; ecx = BSS 大小（字节数）
    xor eax, eax
    cld
    rep stosb             ; 将 [edi] 开始的 ecx 字节清零

    ; 调用内核主函数
    call kernel_main
    jmp $