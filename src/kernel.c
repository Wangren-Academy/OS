// kernel.c
#include "kernel.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "screen.h"

void kernel_main() {
    clear_screen();
    print("MVP OS (Structured Version)\n");
    print("Type something (Enter for new line):\n> ");

    idt_init();
    pic_remap();
    keyboard_init();

    __asm__("sti");

    while (1) {
        char c = keyboard_getchar();
        if (c) {
            putchar(c);           // 回显
            if (c == '\n') {
                print("> ");      // 提示符
            }
        }
        __asm__("hlt");
    }
}