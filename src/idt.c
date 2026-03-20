// idt.c - 中断描述符表初始化
#include "idt.h"
#include "kernel.h"
#include "memory.h"

// IDT 条目结构
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// IDTR 结构
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

// 外部汇编入口
extern void isr0(void);
extern void isr33(void);

// 设置 IDT 条目
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_init(void) {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base  = (uint32_t)&idt;

    // 清空所有 IDT 条目
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // 设置除零异常（中断 0）
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);   // 0x8E = present, ring0, 32-bit interrupt gate
    // 设置键盘中断（IRQ1 → 中断 33）
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);

    // 加载 IDTR
    __asm__ volatile("lidt (%0)" : : "r" (&idtp));
}

// 除零异常处理函数
void exception_handler(void) {
    const char *msg = "Divide by zero!";
    char *video = (char*)VIDEO_MEMORY;
    int i = 0;
    while (*msg) {
        video[i] = *msg++;
        video[i+1] = ERROR_ATTRIBUTE;
        i += 2;
    }
    while (1) {
        __asm__("hlt");
    }
}