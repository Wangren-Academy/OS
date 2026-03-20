// kernel.c - 内核主程序
#include "kernel.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "screen.h"
#include "fs.h"
#include "string.h"

#define CMD_BUFFER_SIZE 128
static char cmd_buffer[CMD_BUFFER_SIZE];
static int  cmd_pos = 0;

// 命令处理函数
static void handle_command(const char *cmd) {
    if (cmd[0] == '\0') return;

    if (strcmp(cmd, "ls") == 0) {
        fs_list();
    } else if (strncmp(cmd, "cat ", 4) == 0) {
        const char *filename = cmd + 4;
        char buffer[256];
        int len = fs_read_file(filename, buffer, sizeof(buffer) - 1);
        if (len < 0) {
            print("File not found.\n");
        } else {
            buffer[len] = '\0';
            print(buffer);
            print("\n");
        }
    } else {
        print("Unknown command.\n");
    }
}

void kernel_main(void) {
    clear_screen();
    print("MVP OS with File System (Static FS)\n");

    idt_init();          // 初始化中断描述符表
    pic_remap();         // 重映射 PIC
    keyboard_init();     // 初始化键盘
    fs_init();           // 初始化文件系统

    print("> ");

    __asm__("sti");      // 开中断

    while (1) {
        char c = keyboard_getchar();
        if (c) {
            if (c == '\n') {
                putchar('\n');
                cmd_buffer[cmd_pos] = '\0';
                handle_command(cmd_buffer);
                cmd_pos = 0;
                print("> ");
            } else if (c == '\b') {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    putchar('\b');
                }
            } else if (cmd_pos < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_pos++] = c;
                putchar(c);  // 回显
            }
        }
        __asm__("hlt");   // 节省 CPU 功耗
    }
}