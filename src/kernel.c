// src/kernel.c
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
    // 0x08 是代码段选择子（你的boot.asm中CODE_SEG = 0x08）
    // 0x8E 表示 present, ring0, 32位中断门
    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);

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
    // 直接写显存 - 现在是在保护模式下，32位地址直接可用
    char *video_memory = (char*)0xb8000;
    const char *message = "Hello MVP OS! Running in Protected Mode!";
    int i = 0;
    
    // 清屏
    for (int j = 0; j < 80 * 25 * 2; j += 2) {
        video_memory[j] = ' ';
        video_memory[j + 1] = 0x07;
    }
    
    // 显示消息
    while (*message) {
        video_memory[i] = *message++;
        video_memory[i + 1] = 0x07;  // 白色
        i += 2;
    }
    
    // 初始化IDT
    init_idt();

    // 故意除零
    int a = 10;
    int b = 0;
    int c = a / b;   // 触发中断0
    
    // 无限循环
    while(1) {
        __asm__ volatile("hlt");
    }
}