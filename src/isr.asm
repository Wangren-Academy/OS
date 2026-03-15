[bits 32]

global isr0
extern exception_handler

isr0:
    cli                     ; 关中断
    pusha                   ; 保存所有通用寄存器
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10            ; 数据段选择子（boot.asm中DATA_SEG = 0x10）
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call exception_handler  ; 调用C函数
    pop gs
    pop fs
    pop es
    pop ds
    popa
    sti                     ; 开中断
    iret                    ; 中断返回

; 键盘中断 (IRQ1 映射到中断33)
global isr33
extern keyboard_handler
isr33:
    cli
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call keyboard_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    sti
    iret