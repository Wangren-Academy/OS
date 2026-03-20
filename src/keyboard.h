// keyboard.h - 键盘驱动接口
#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init(void);
void keyboard_handler(void);           // 由汇编中断调用
char keyboard_getchar(void);           // 从缓冲区读取一个字符（非阻塞）
int  keyboard_data_available(void);    // 检查缓冲区是否有数据

#endif