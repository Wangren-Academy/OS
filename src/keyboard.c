// keyboard.c
#include "keyboard.h"
#include "io.h"
#include "scancodes.h"
#include "kernel.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64
#define PIC1_COMMAND         0x20

#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];
static int head = 0, tail = 0;

static int shift_pressed = 0;
static int caps_lock = 0;

// 扫描码映射表（无Shift）
static const char normal_map[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// 扫描码映射表（Shift按下）
static const char shift_map[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

#define MAP_SIZE (sizeof(normal_map) / sizeof(normal_map[0]))

static void buffer_put(char c) {
    int next = (head + 1) % BUFFER_SIZE;
    if (next != tail) {   // 缓冲区未满
        buffer[head] = c;
        head = next;
    }
}

void keyboard_init() {
    // 无需额外初始化
}

char keyboard_getchar() {
    if (head == tail) return 0;
    char c = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return c;
}

int keyboard_data_available() {
    return head != tail;
}

void keyboard_handler() {
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (!(status & 1)) goto eoi;   // 无数据

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // 处理Shift键
    if (scancode == KEY_LEFT_SHIFT || scancode == KEY_RIGHT_SHIFT) {
        shift_pressed = 1;
    } else if (scancode == KEY_LEFT_SHIFT_REL || scancode == KEY_RIGHT_SHIFT_REL) {
        shift_pressed = 0;
    }
    // 处理Caps Lock
    else if (scancode == KEY_CAPS_LOCK) {
        caps_lock = !caps_lock;
    }
    // 处理退格
    else if (scancode == KEY_BACKSPACE) {
        buffer_put('\b');
    }
    // 处理回车
    else if (scancode == KEY_ENTER) {
        buffer_put('\n');
    }
    // 普通按键（按下事件，忽略松开）
    else if (!(scancode & 0x80) && scancode < MAP_SIZE) {
        char ascii = shift_pressed ? shift_map[scancode] : normal_map[scancode];
        if (caps_lock && ascii >= 'a' && ascii <= 'z') {
            ascii -= 'a' - 'A';
        }
        if (ascii) {
            buffer_put(ascii);
        }
    }

eoi:
    // 发送EOI到主PIC
    outb(PIC1_COMMAND, 0x20);
}