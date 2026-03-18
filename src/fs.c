// fs.c
#include "fs.h"
#include "screen.h"
#include "kernel.h"

// ---------- 静态文件数据 ----------
static const char file1_data[] = "Hello World!\n";
static const char file2_data[] = "This is a simple readme file.\nLine 2.\n";

// 数据块指针数组（索引即 start_block）
static const char *data_blocks[] = {
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

// 超级块（仅用于标识）
static superblock_t sb = {
    .magic = FS_MAGIC,
    .version = FS_VERSION,
    .root_block = 0,
    .data_block = 0,
    .total_blocks = NUM_BLOCKS
};

// 简单字符串比较（供内部使用）
static int str_eq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a == *b;
}

void fs_init() {
    print("FS: Built-in file system ready.\n");
}

void fs_list() {
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
    for (int i = 0; i < MAX_FILES && root_dir[i].name[0] != '\0'; i++) {
        if (str_eq(name, root_dir[i].name)) {
            uint32_t size = root_dir[i].size;
            uint32_t to_read = (size < maxlen) ? size : maxlen;
            uint32_t block = root_dir[i].start_block;
            if (block >= NUM_BLOCKS) return -1;          // 索引错误
            const char *src = data_blocks[block];
            for (uint32_t j = 0; j < to_read; j++) {
                buffer[j] = src[j];
            }
            return to_read;
        }
    }
    return -1;  // 文件未找到
}