; boot.asm - 引导加载程序 (实模式)
[org 0x7c00]

; 常量定义
KERNEL_STACK_TOP equ 0x90000          ; 内核栈顶（保护模式使用）
KERNEL_PHYS_ADDR equ 0x10000          ; 内核加载物理地址
KERNEL_SEG       equ 0x1000           ; 内核段地址（实模式）
KERNEL_OFF       equ 0x0000           ; 内核段内偏移
STACK_TOP        equ 0x7c00           ; 临时实模式栈顶
KERNEL_SECTORS   equ 100               ; 加载的扇区数（需根据实际内核大小调整）

[bits 16]

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, STACK_TOP

; 保存启动驱动器号
mov [boot_drive], dl

; 清屏（设置为文本模式）
mov ax, 0x0003
int 0x10

; 打印启动信息
mov si, msg_boot
call print_string

; 重置磁盘系统
mov ah, 0
int 0x13

; 从磁盘加载内核到 KERNEL_SEG:KERNEL_OFF
mov ax, KERNEL_SEG
mov es, ax
xor bx, KERNEL_OFF

mov ah, 0x02          ; 读扇区功能
mov al, KERNEL_SECTORS; 要读的扇区数
mov ch, 0             ; 柱面 0
mov cl, 2             ; 起始扇区 2（扇区1是引导扇区）
mov dh, 0             ; 磁头 0
mov dl, [boot_drive]  ; 驱动器号
int 0x13

jc disk_error         ; 如果出错，跳转

; 加载 GDTR
lgdt [gdt_descriptor]

; 进入保护模式
mov eax, cr0
or eax, 0x1
mov cr0, eax
jmp CODE_SEG:init_protected_mode

[bits 32]
init_protected_mode:
    ; 设置数据段寄存器
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 设置内核栈
    mov esp, KERNEL_STACK_TOP

    ; 跳转到内核入口
    call KERNEL_PHYS_ADDR

    ; 内核不应返回，若返回则死循环
    jmp $

[bits 16]
disk_error:
    mov si, msg_error
    call print_string
    jmp $

; 打印字符串（以0结尾）
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

; GDT (全局描述符表)
gdt_start:
    dd 0x0, 0x0                     ; 空描述符
gdt_code:
    dw 0xFFFF                       ; 段限长 0-15
    dw 0x0                           ; 基址 0-15
    db 0x0                           ; 基址 16-23
    db 10011010b                     ; 存在、环0、代码段、可执行、可读
    db 11001111b                     ; 粒度4K、32位、段限长 16-19
    db 0x0                           ; 基址 24-31
gdt_data:
    dw 0xFFFF                       ; 段限长 0-15
    dw 0x0                           ; 基址 0-15
    db 0x0                           ; 基址 16-23
    db 10010010b                     ; 存在、环0、数据段、可写
    db 11001111b                     ; 粒度4K、32位、段限长 16-19
    db 0x0                           ; 基址 24-31
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1       ; GDT 限长
    dd gdt_start                     ; GDT 基址

; 段选择子常量
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; 数据区
boot_drive db 0
msg_boot    db 'Loading MVP OS...', 13, 10, 0
msg_loading db 'Reading kernel from disk...', 13, 10, 0
msg_ok      db 'OK! Entering protected mode...', 13, 10, 0
msg_error   db 'Error: Disk read failed!', 13, 10, 0

; 填充至512字节，并添加引导签名
times 510-($-$$) db 0
dw 0xaa55