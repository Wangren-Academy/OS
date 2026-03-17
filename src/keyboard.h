// keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init();
void keyboard_handler();           // 汇编调用
char keyboard_getchar();           // 从缓冲区取一个字符
int keyboard_data_available();     // 检查缓冲区是否有数据

#endif