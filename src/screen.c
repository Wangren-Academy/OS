// src/screen.c
#include "screen.h"

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static char *video_memory = (char*)0xb8000;

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i]   = ' ';
        video_memory[i+1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void putchar(char c) {
    int pos = (cursor_y * 80 + cursor_x) * 2;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            pos = (cursor_y * 80 + cursor_x) * 2;
            video_memory[pos]   = ' ';
            video_memory[pos+1] = 0x07;
        }
        // 可扩展：处理行首退格到上一行
    } else {
        video_memory[pos]   = c;
        video_memory[pos+1] = 0x07;
        cursor_x++;
    }

    // 处理行末换行
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // 滚动屏幕
    if (cursor_y >= 25) {
        // 将第1-24行上移一行
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 80; x++) {
                int from = ((y+1) * 80 + x) * 2;
                int to   = (y * 80 + x) * 2;
                video_memory[to]   = video_memory[from];
                video_memory[to+1] = video_memory[from+1];
            }
        }
        // 清空最后一行
        for (int x = 0; x < 80; x++) {
            int last = (24 * 80 + x) * 2;
            video_memory[last]   = ' ';
            video_memory[last+1] = 0x07;
        }
        cursor_y = 24;
    }
}

void print(const char *str) {
    while (*str) {
        putchar(*str++);
    }
}