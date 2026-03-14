# OS

sudo apt install -y \
    nasm \
    qemu-system-x86 \
    gcc

## 1. 编译bootloader（生成二进制文件）
nasm -f bin src/boot.asm -o build/boot.bin

## 2. 编译内核（生成32位目标文件）
gcc -m32 -ffreestanding -fno-pie -c src/kernel.c -o build/kernel.o

## 3. 链接内核（生成二进制文件）
ld -m elf_i386 -T src/linker.ld build/kernel.o -o build/kernel.bin

## 4. 合并成完整系统镜像
cat build/boot.bin build/kernel.bin > build/os-image.bin

## 5. 运行
qemu-system-x86_64 -drive format=raw,file=build/os-image.bin