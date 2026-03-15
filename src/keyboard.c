// src/keyboard.c - 键盘驱动
#include "io.h"
#include <stdint.h>

#define PIC1_COMMAND 0x20
// 键盘控制器端口
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// 键盘按键映射表（简化版，只处理字母和数字）
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

// 键盘状态
static int shift_pressed = 0;
static int caps_lock = 0;

// 屏幕位置
static int cursor_x = 0;
static int cursor_y = 0;
static char *video_memory = (char*)0xb8000;

// 初始化键盘
void init_keyboard() {
    cursor_x = 0;
    cursor_y = 0;
}

// 在屏幕上显示一个字符
void putchar(char c) {
    int pos = (cursor_y * 80 + cursor_x) * 2;
    
    if (c == '\n') {
        // 换行
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b' && cursor_x > 0) {
        // 退格
        cursor_x--;
        pos = (cursor_y * 80 + cursor_x) * 2;
        video_memory[pos] = ' ';
        video_memory[pos + 1] = 0x07;
    } else {
        // 正常字符
        video_memory[pos] = c;
        video_memory[pos + 1] = 0x07;
        cursor_x++;
    }
    
    // 换行处理
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    
    // 滚动屏幕（如果到底部）
    if (cursor_y >= 25) {
        // 把第1-24行上移一行
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 80; x++) {
                int from = ((y + 1) * 80 + x) * 2;
                int to = (y * 80 + x) * 2;
                video_memory[to] = video_memory[from];
                video_memory[to + 1] = video_memory[from + 1];
            }
        }
        // 清空最后一行
        for (int x = 0; x < 80; x++) {
            int pos = (24 * 80 + x) * 2;
            video_memory[pos] = ' ';
            video_memory[pos + 1] = 0x07;
        }
        cursor_y = 24;
    }
}

// 键盘中断处理函数
void keyboard_handler() {
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    
    // 检查是否有数据可读
    if (status & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        
        // 处理Shift键
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
            return;
        }
        if (scancode == 0xAA || scancode == 0xB6) {
            shift_pressed = 0;
            return;
        }
        
        // 处理Caps Lock
        if (scancode == 0x3A) {
            caps_lock = !caps_lock;
            return;
        }
        
        // 处理退格键
        if (scancode == 0x0E) {
            putchar('\b');
            return;
        }
        
        // 处理回车键
        if (scancode == 0x1C) {
            putchar('\n');
            return;
        }
        
        // 只处理按键按下（不处理释放）
        if (!(scancode & 0x80)) {
            char ascii;
            
            // 根据Shift和Caps Lock选择映射表
            if (shift_pressed) {
                ascii = scancode_to_ascii_shift[scancode];
            } else {
                ascii = scancode_to_ascii[scancode];
            }
            
            // 处理Caps Lock（只影响字母）
            if (caps_lock && ascii >= 'a' && ascii <= 'z') {
                ascii = ascii - 'a' + 'A';
            }
            
            if (ascii) {
                putchar(ascii);
            }
        }
    }

    // 发送 EOI 到主片（因为键盘是 IRQ1，属于主片）
    outb(PIC1_COMMAND, 0x20);
}