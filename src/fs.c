// fs.c - 静态文件系统实现
#include "fs.h"
#include "screen.h"
#include "string.h"
#include "kernel.h"

// ---------- 静态文件数据 ----------
static const char file1_data[] = "Hello World!\n";
static const char file2_data[] = "This is a simple readme file.\nLine 2.\n";

// 数据块指针数组（索引即 start_block）
static const char * const data_blocks[] = {
    file1_data,
    file2_data
};
#define NUM_BLOCKS (sizeof(data_blocks) / sizeof(data_blocks[0]))

// 根目录表（以空名字结尾）
static dir_entry_t root_dir[MAX_FILES] = {
    { .name = "hello.txt", .attributes = 0, .reserved = 0, .start_block = 0, .size = sizeof(file1_data) - 1 },
    { .name = "readme",    .attributes = 0, .reserved = 0, .start_block = 1, .size = sizeof(file2_data) - 1 },
    { .name = "", .attributes = 0, .reserved = 0, .start_block = 0, .size = 0 }  // 终止项
};

// 超级块
static const superblock_t sb = {
    .magic = FS_MAGIC,
    .version = FS_VERSION,
    .root_block = 0,
    .data_block = 0,
    .total_blocks = NUM_BLOCKS
};

void fs_init(void) {
    print("FS: Built-in file system ready.\n");
}

void fs_list(void) {
    int found = 0;
    for (int i = 0; i < MAX_FILES && root_dir[i].name[0] != '\0'; i++) {
        print(root_dir[i].name);
        print("  ");
        found = 1;
    }
    if (!found) {
        print("(empty)");
    }
    print("\n");
}

int fs_read_file(const char *name, char *buffer, uint32_t maxlen) {
    if (!name || !buffer) return -1;

    for (int i = 0; i < MAX_FILES && root_dir[i].name[0] != '\0'; i++) {
        // 安全比较文件名（最多 23 个字符，因为 name[24] 可能无终止符）
        if (strncmp(name, root_dir[i].name, 23) == 0) {
            uint32_t size = root_dir[i].size;
            uint32_t to_read = (size < maxlen) ? size : maxlen;
            uint32_t block = root_dir[i].start_block;
            if (block >= NUM_BLOCKS) return -1;          // 块索引错误

            const char *src = data_blocks[block];
            for (uint32_t j = 0; j < to_read; j++) {
                buffer[j] = src[j];
            }
            return (int)to_read;
        }
    }
    return -1;  // 文件未找到
}