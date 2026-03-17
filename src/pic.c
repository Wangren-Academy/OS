// pic.c
#include "pic.h"
#include "io.h"
#include "kernel.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_remap() {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);      // 主片中断从32开始
    outb(PIC2_DATA, 0x28);      // 从片中断从40开始
    outb(PIC1_DATA, 0x04);      // 主片有从片在IRQ2
    outb(PIC2_DATA, 0x02);      // 从片级联ID为2
    outb(PIC1_DATA, 0x01);      // 8086模式
    outb(PIC2_DATA, 0x01);

    // 允许键盘中断 (IRQ1)，屏蔽其他
    outb(PIC1_DATA, 0xFD);      // 11111101
    outb(PIC2_DATA, 0xFF);      // 屏蔽所有从片中断
}