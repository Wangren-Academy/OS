// src/kernel.c
#include "io.h"
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// 声明外部函数
extern void isr33();  // 键盘中断

// PIC初始化函数
void pic_remap() {
    // 保存原来的屏蔽字
    unsigned char a1 = inb(PIC1_DATA);
    unsigned char a2 = inb(PIC2_DATA);

    // 初始化主片和从片
    outb(PIC1_COMMAND, 0x11);  // ICW1: 初始化
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);      // ICW2: 主片中断从32开始
    outb(PIC2_DATA, 0x28);      // ICW2: 从片中断从40开始
    outb(PIC1_DATA, 0x04);      // ICW3: 主片有从片在IRQ2
    outb(PIC2_DATA, 0x02);      // ICW3: 从片级联ID为2
    outb(PIC1_DATA, 0x01);      // ICW4: 8086模式
    outb(PIC2_DATA, 0x01);

    // 显式设置中断屏蔽字，只允许键盘中断 (IRQ1)
    outb(PIC1_DATA, 0xFD);  // 11111101，只允许 IRQ1（键盘），屏蔽其他
    outb(PIC2_DATA, 0xFF);  // 11111111，屏蔽所有从片中断
}

// 定义IDT条目结构
struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

// IDTR结构
struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

// 声明IDT和IDTR
struct idt_entry idt[256];
struct idt_ptr idtp;

// 外部汇编函数声明
extern void isr0();   // 除零异常的处理函数

// 设置IDT条目的函数
void idt_set_gate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// 初始化IDT
void init_idt() {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base = (unsigned int)&idt;

    // 清空IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // 设置除零异常（中断号0）的处理函数
    // 0x08 是代码段选择子（boot.asm中CODE_SEG = 0x08）
    // 0x8E 表示 present, ring0, 32位中断门
    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)isr33, 0x08, 0x8E);  // 键盘中断

    // 加载IDTR
    __asm__ volatile("lidt (%0)" : : "r" (&idtp));
}

// 异常处理C函数（由汇编调用）
void exception_handler() {
    // 在屏幕显示错误信息
    char *video = (char*)0xb8000;
    char *msg = "Divide by zero!";
    int i = 0;
    while (*msg) {
        video[i] = *msg++;
        video[i+1] = 0x04; // 红色
        i += 2;
    }
    // 死循环
    while(1) {
        __asm__ volatile("hlt");
    }
}

void kernel_main() {
    char *video_memory = (char*)0xb8000;
    const char *message = "MVP OS with Keyboard Support!\nType something...\n";
    int i = 0;
    
    // 清屏
    for (int j = 0; j < 80 * 25 * 2; j += 2) {
        video_memory[j] = ' ';
        video_memory[j + 1] = 0x07;
    }
    
    // 显示欢迎信息
    while (*message) {
        if (*message == '\n') {
            // 简单的换行处理（临时）
            i = ((i / 160) + 1) * 160;
            message++;
        } else {
            video_memory[i] = *message++;
            video_memory[i + 1] = 0x07;
            i += 2;
        }
    }
    
    // 初始化IDT
    init_idt();
    
    // 重新映射PIC
    pic_remap();
    
    // 初始化键盘
    // 注意：keyboard.c中的init_keyboard需要声明为外部函数
    // 这里简单起见，我们直接用putchar显示
    extern void init_keyboard();
    init_keyboard();
    
    // 开启中断
    __asm__ volatile("sti");
    
    // 无限循环，中断会处理键盘
    while(1) {
        __asm__ volatile("hlt");
    }
}