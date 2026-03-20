// string.c - 字符串操作实现
#include "string.h"

int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, int n) {
    while (n-- && *a && *b && *a == *b) {
        a++;
        b++;
    }
    if (n < 0) return 0;
    return (unsigned char)*a - (unsigned char)*b;
}