; src/kernel_entry.asm
[bits 32]          ; 明确指定为32位模式
[extern kernel_main]  ; 引用C函数

global _start
_start:
    ; 内核入口点
    call kernel_main   ; 调用C函数
    jmp $              ; 不应该返回