; boot.asm
[org 0x7c00]
KERNEL_STACK_TOP equ 0x90000
KERNEL_PHYS_ADDR equ 0x10000

[bits 16]

KERNEL_SEG equ 0x1000       ; 内核段地址
KERNEL_OFF equ 0x0000       ; 内核偏移
STACK_TOP  equ 0x7c00       ; 临时栈顶
KERNEL_SECTORS equ 100      ; 加载扇区数（可根据实际修改）

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, STACK_TOP

; 保存启动驱动器号
mov [boot_drive], dl

; 清屏
mov ax, 0x0003
int 0x10

; 打印启动信息
mov si, msg_boot
call print_string

; 打印加载信息
mov si, msg_loading
call print_string

; 重置磁盘系统
mov ah, 0
int 0x13

; 加载内核
mov ax, KERNEL_SEG
mov es, ax
xor bx, KERNEL_OFF

mov ah, 0x02
mov al, KERNEL_SECTORS
mov ch, 0
mov cl, 2           ; 从第2个扇区开始
mov dh, 0
mov dl, [boot_drive]
int 0x13

jc disk_error

mov si, msg_ok
call print_string

; 加载GDTR
lgdt [gdt_descriptor]

; 进入保护模式
mov eax, cr0
or eax, 0x1
mov cr0, eax
jmp CODE_SEG:init_protected_mode

[bits 32]
init_protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, KERNEL_STACK_TOP   ; 使用内核栈（注意：此处地址在32位下是0x90000）
    call KERNEL_PHYS_ADDR       ; 跳转到内核
    jmp $

[bits 16]
disk_error:
    mov si, msg_error
    call print_string
    jmp $

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

; GDT
gdt_start:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

boot_drive db 0
msg_boot db 'Loading MVP OS...', 13, 10, 0
msg_loading db 'Reading kernel from disk...', 13, 10, 0
msg_ok db 'OK! Entering protected mode...', 13, 10, 0
msg_error db 'Error: Disk read failed!', 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55