// src/kernel.c
#include "kernel.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "screen.h"

void kernel_main() {
    // 清屏并显示欢迎信息
    clear_screen();
    print("MVP OS with Keyboard Support!\n");
    print("Type something...\n");

    // 初始化IDT
    init_idt();

    // 重新映射PIC
    pic_remap();

    // 初始化键盘
    init_keyboard();

    // 开启中断
    __asm__ volatile("sti");

    // 无限循环，中断处理键盘输入
    while (1) {
        __asm__ volatile("hlt");
    }
}