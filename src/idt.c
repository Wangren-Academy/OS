// idt.c
#include "idt.h"
#include "kernel.h"
#include "memory.h"

// IDT条目结构
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// IDTR结构
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

// 外部汇编函数声明
extern void isr0();   // 除零异常
extern void isr33();  // 键盘中断

// 设置IDT条目
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_init() {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base  = (uint32_t)&idt;

    // 清空所有IDT条目
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // 设置除零异常（中断0）
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    // 设置键盘中断（IRQ1 → 中断33）
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);

    // 加载IDTR
    __asm__ volatile("lidt (%0)" : : "r" (&idtp));
}

// 异常处理函数（被isr0调用）
void exception_handler() {
    const char *msg = "Divide by zero!";
    char *video = VIDEO_MEMORY;
    int i = 0;
    while (*msg) {
        video[i] = *msg++;
        video[i+1] = ERROR_ATTRIBUTE;
        i += 2;
    }
    while (1) __asm__("hlt");
}