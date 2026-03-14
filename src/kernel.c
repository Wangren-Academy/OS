// src/kernel.c
void kernel_main() {
    // 直接写显存 - 现在是在保护模式下，32位地址直接可用
    char *video_memory = (char*)0xb8000;
    const char *message = "Hello MVP OS! Running in Protected Mode!";
    int i = 0;
    
    // 清屏
    for (int j = 0; j < 80 * 25 * 2; j += 2) {
        video_memory[j] = ' ';
        video_memory[j + 1] = 0x07;
    }
    
    // 显示消息
    while (*message) {
        video_memory[i] = *message++;
        video_memory[i + 1] = 0x07;  // 白色
        i += 2;
    }
    
    // 无限循环
    while(1) {
        __asm__ volatile("hlt");
    }
}