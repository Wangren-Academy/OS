// src/idt.h
#ifndef IDT_H
#define IDT_H

void init_idt();
void exception_handler();  // 被汇编调用

#endif