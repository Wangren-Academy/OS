// src/kernel.c
void kernel_main() {
    // 直接写显存
    char *video_memory = (char*)0xb8000;
    
    // 显示文字
    video_memory[0] = 'H';
    video_memory[1] = 0x07;  // 白色
    video_memory[2] = 'e';
    video_memory[3] = 0x07;
    video_memory[4] = 'l';
    video_memory[5] = 0x07;
    video_memory[6] = 'l';
    video_memory[7] = 0x07;
    video_memory[8] = 'o';
    video_memory[9] = 0x07;
    video_memory[10] = ' ';
    video_memory[11] = 0x07;
    video_memory[12] = 'O';
    video_memory[13] = 0x07;
    video_memory[14] = 'S';
    video_memory[15] = 0x07;
    
    // 死循环
    while(1) {
        __asm__("hlt");
    }
}