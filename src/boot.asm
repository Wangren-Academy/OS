; src/boot.asm
[org 0x7c00]
[bits 16]

; 初始化段寄存器
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 保存启动驱动器号
mov [boot_drive], dl

; 清屏
mov ax, 0x0003
int 0x10

; 打印启动信息
mov si, msg_boot
call print_string

; 加载内核到内存 0x1000:0x0000 (物理地址 0x10000)
mov si, msg_loading
call print_string

; 重置磁盘系统
mov ah, 0
int 0x13

; 加载内核 (从第2个扇区开始，加载50个扇区，约25KB)
mov ax, 0x1000
mov es, ax
xor bx, bx

mov ah, 0x02
mov al, 50        ; 加载50个扇区（可根据实际内核大小调整）
mov ch, 0
mov cl, 2         ; 从第2个扇区开始
mov dh, 0
mov dl, [boot_drive]
int 0x13

; 检查错误
jc disk_error

; 打印成功信息
mov si, msg_ok
call print_string

; 加载GDTR
lgdt [gdt_descriptor]

; 进入保护模式
mov eax, cr0
or eax, 0x1        ; 设置PE位
mov cr0, eax

; 远跳转刷新流水线，加载代码段选择子
jmp CODE_SEG:init_protected_mode

[bits 32]
init_protected_mode:
    ; 初始化数据段寄存器
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 设置栈指针 (使用内核栈区域)
    mov esp, 0x90000

    ; 跳转到内核 (内核加载在 0x10000)
    call 0x10000

    ; 如果内核返回，死循环
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

; GDT (全局描述符表)
gdt_start:
    ; 空描述符
    dd 0x0
    dd 0x0

; 代码段描述符
gdt_code:
    dw 0xFFFF       ; 段界限 0-15
    dw 0x0          ; 基地址 0-15
    db 0x0          ; 基地址 16-23
    db 10011010b    ; 存在、特权级0、代码段、可执行、可读
    db 11001111b    ; 4K粒度、32位、段界限16-19
    db 0x0          ; 基地址 24-31

; 数据段描述符
gdt_data:
    dw 0xFFFF       ; 段界限 0-15
    dw 0x0          ; 基地址 0-15
    db 0x0          ; 基地址 16-23
    db 10010010b    ; 存在、特权级0、数据段、可读写
    db 11001111b    ; 4K粒度、32位、段界限16-19
    db 0x0          ; 基地址 24-31

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT大小减1
    dd gdt_start                ; GDT起始地址

; 常量
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

boot_drive db 0
msg_boot db 'Loading MVP OS...', 13, 10, 0
msg_loading db 'Reading kernel from disk...', 13, 10, 0
msg_ok db 'OK! Entering protected mode...', 13, 10, 0
msg_error db 'Error: Disk read failed!', 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55