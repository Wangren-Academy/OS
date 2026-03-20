// fs.h - 文件系统头文件
#ifndef FS_H
#define FS_H

#include "kernel.h"

#define FS_MAGIC        0x4D565046   // "MVPS"
#define FS_VERSION      1
#define BLOCK_SIZE      512
#define DIR_ENTRY_SIZE  32
#define MAX_FILES       128          // 最多支持 128 个文件

// 超级块（静态文件系统仅用于标识）
typedef struct {
    uint32_t magic;
    uint8_t  version;
    uint32_t root_block;      // 未使用
    uint32_t data_block;      // 未使用
    uint32_t total_blocks;    // 数据块数量
} __attribute__((packed)) superblock_t;

// 目录项
typedef struct {
    char     name[24];        // 文件名，以 '\0' 结尾（若占满则无终止符）
    uint8_t  attributes;      // 文件属性（未使用）
    uint8_t  reserved;        // 保留
    uint16_t start_block;     // 数据块索引
    uint32_t size;            // 文件大小（字节）
} __attribute__((packed)) dir_entry_t;

void fs_init(void);
void fs_list(void);
int  fs_read_file(const char *name, char *buffer, uint32_t maxlen);

#endif