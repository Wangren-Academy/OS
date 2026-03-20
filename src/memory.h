// memory.h - 内存相关常量
#ifndef MEMORY_H
#define MEMORY_H

// 视频显存 (文本模式)
#define VIDEO_MEMORY        0xb8000
#define VIDEO_WIDTH         80
#define VIDEO_HEIGHT        25
#define VIDEO_ATTRIBUTE     0x07   // 白底黑字

// 内核加载地址
#define KERNEL_PHYS_ADDR    0x10000   // 物理地址
#define KERNEL_STACK_TOP    0x90000   // 内核栈顶

// 异常信息颜色
#define ERROR_ATTRIBUTE     0x04      // 红色

#endif