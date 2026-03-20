; isr.asm - 中断服务例程（保护模式）
[bits 32]

global isr0
global isr33

extern exception_handler
extern keyboard_handler

; 除零异常 (中断 0)
isr0:
    cli
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10            ; 内核数据段选择子
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call exception_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    sti
    iret

; 键盘中断 (IRQ1 → 中断 33)
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