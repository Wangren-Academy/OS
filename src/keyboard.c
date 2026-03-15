// src/keyboard.c
#include "keyboard.h"
#include "io.h"
#include "screen.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64
#define PIC1_COMMAND         0x20

// 扫描码到ASCII映射（无Shift）
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// 扫描码到ASCII映射（Shift按下）
static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

static int shift_pressed = 0;
static int caps_lock     = 0;

void init_keyboard() {
    // 无额外初始化
}

void keyboard_handler() {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);

    if (status & 1) {  // 输出缓冲区非空
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        // 处理Shift键
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
            goto eoi;
        }
        if (scancode == 0xAA || scancode == 0xB6) {
            shift_pressed = 0;
            goto eoi;
        }

        // 处理Caps Lock
        if (scancode == 0x3A) {
            caps_lock = !caps_lock;
            goto eoi;
        }

        // 处理退格键
        if (scancode == 0x0E) {
            putchar('\b');
            goto eoi;
        }

        // 处理回车键
        if (scancode == 0x1C) {
            putchar('\n');
            goto eoi;
        }

        // 只处理按键按下（忽略松开）
        if (!(scancode & 0x80)) {
            char ascii;
            if (shift_pressed)
                ascii = scancode_to_ascii_shift[scancode];
            else
                ascii = scancode_to_ascii[scancode];

            // Caps Lock影响字母
            if (caps_lock && ascii >= 'a' && ascii <= 'z')
                ascii = ascii - 'a' + 'A';

            if (ascii)
                putchar(ascii);
        }
    }

eoi:
    // 发送EOI到主片
    outb(PIC1_COMMAND, 0x20);
}