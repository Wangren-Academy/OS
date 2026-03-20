// pic.c - 8259A 初始化
#include "pic.h"
#include "io.h"
#include "kernel.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_remap(void) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    // 初始化主片和从片
    outb(PIC1_COMMAND, 0x11);   // ICW1: 初始化
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);       // ICW2: 主片中断基址 32
    outb(PIC2_DATA, 0x28);       // ICW2: 从片中断基址 40
    outb(PIC1_DATA, 0x04);       // ICW3: 主片有从片在 IRQ2
    outb(PIC2_DATA, 0x02);       // ICW3: 从片级联到主片 IRQ2
    outb(PIC1_DATA, 0x01);       // ICW4: 8086 模式
    outb(PIC2_DATA, 0x01);

    // 屏蔽中断：只允许键盘中断 (IRQ1)，其他全部屏蔽
    outb(PIC1_DATA, 0xFD);       // 11111101 (IRQ1 允许)
    outb(PIC2_DATA, 0xFF);       // 屏蔽所有从片中断
}