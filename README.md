# OS

sudo apt install -y \
    nasm \
    qemu-system-x86 \
    gcc

## 编译引导扇区
nasm -f bin src/boot.asm -o build/boot.bin

## 编译内核入口
nasm -f elf32 src/kernel_entry.asm -o build/kernel_entry.o

## 编译中断处理
nasm -f elf32 src/isr.asm -o build/isr.o

## 编译C内核
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c src/kernel.c -o build/kernel.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c src/keyboard.c -o build/keyboard.o

## 链接内核
ld -m elf_i386 -T src/linker.ld build/kernel_entry.o build/kernel.o build/keyboard.o build/isr.o -o build/kernel.bin

## 创建镜像
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null

## 运行
qemu-system-i386 -drive format=raw,file=build/os.img