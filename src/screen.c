// screen.c - 屏幕输出（80x25 文本模式）
#include "screen.h"
#include "memory.h"
#include "kernel.h"

static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static char *video_memory = (char*)VIDEO_MEMORY;

void clear_screen(void) {
    for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT * 2; i += 2) {
        video_memory[i]   = ' ';
        video_memory[i+1] = VIDEO_ATTRIBUTE;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int pos = (cursor_y * VIDEO_WIDTH + cursor_x) * 2;
            video_memory[pos]   = ' ';
            video_memory[pos+1] = VIDEO_ATTRIBUTE;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VIDEO_WIDTH - 1;
            int pos = (cursor_y * VIDEO_WIDTH + cursor_x) * 2;
            video_memory[pos]   = ' ';
            video_memory[pos+1] = VIDEO_ATTRIBUTE;
        }
    } else {
        int pos = (cursor_y * VIDEO_WIDTH + cursor_x) * 2;
        video_memory[pos]   = c;
        video_memory[pos+1] = VIDEO_ATTRIBUTE;
        cursor_x++;
    }

    // 处理行换行
    if (cursor_x >= VIDEO_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // 处理屏幕滚动
    if (cursor_y >= VIDEO_HEIGHT) {
        // 向上滚动一行
        for (int y = 0; y < VIDEO_HEIGHT - 1; y++) {
            for (int x = 0; x < VIDEO_WIDTH; x++) {
                int from = ((y+1) * VIDEO_WIDTH + x) * 2;
                int to   = (y * VIDEO_WIDTH + x) * 2;
                video_memory[to]   = video_memory[from];
                video_memory[to+1] = video_memory[from+1];
            }
        }
        // 清空最后一行
        for (int x = 0; x < VIDEO_WIDTH; x++) {
            int last = ((VIDEO_HEIGHT-1) * VIDEO_WIDTH + x) * 2;
            video_memory[last]   = ' ';
            video_memory[last+1] = VIDEO_ATTRIBUTE;
        }
        cursor_y = VIDEO_HEIGHT - 1;
    }
}

void print(const char *str) {
    while (*str) {
        putchar(*str++);
    }
}