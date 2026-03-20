// pic.h - 可编程中断控制器 (8259A)
#ifndef PIC_H
#define PIC_H

void pic_remap(void);   // 重映射 IRQ 到中断向量 32-47

#endif