; src/boot.asm
[org 0x7c00]
[bits 16]

; 初始化
mov ax, 0
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

; 清屏
mov ax, 0x0003
int 0x10

; 打印信息
mov si, msg_boot
call print_string

; 加载内核
mov ax, 0x1000
mov es, ax
xor bx, bx
mov ah, 0x02
mov al, 1
mov ch, 0
mov cl, 2
mov dh, 0
int 0x13

; 跳转到内核
jmp 0x1000:0x0000

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

msg_boot db 'Loading MVP OS...', 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55