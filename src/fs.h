// fs.h
#ifndef FS_H
#define FS_H

#include "kernel.h"

#define FS_MAGIC 0x4D565046   // "MVPS"
#define FS_VERSION 1
#define BLOCK_SIZE 512

#define DIR_ENTRY_SIZE 32
#define MAX_FILES 128          // 最多支持 128 个文件

typedef struct {
    uint32_t magic;
    uint8_t  version;
    uint32_t root_block;
    uint32_t data_block;
    uint32_t total_blocks;
} __attribute__((packed)) superblock_t;

typedef struct {
    char     name[24];
    uint8_t  attributes;
    uint8_t  reserved;
    uint16_t start_block;      // 数据块索引（在静态实现中指向数据数组）
    uint32_t size;
} __attribute__((packed)) dir_entry_t;

void fs_init();
void fs_list();
int  fs_read_file(const char *name, char *buffer, uint32_t maxlen);

#endif