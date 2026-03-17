; isr.asm
[bits 32]

global isr0
extern exception_handler

isr0:
    cli
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10            ; 数据段选择子
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