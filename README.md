# OS

sudo apt install -y \
    nasm \
    qemu-system-x86 \
    gcc

mkdir -p build
nasm -f bin src/boot.asm -o build/boot.bin
nasm -f elf32 src/kernel_entry.asm -o build/kernel_entry.o
nasm -f elf32 src/isr.asm -o build/isr.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/kernel.c -o build/kernel.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/idt.c -o build/idt.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/pic.c -o build/pic.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/keyboard.c -o build/keyboard.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/screen.c -o build/screen.o
gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -I src -c src/fs.c -o build/fs.o
ld -m elf_i386 -T src/linker.ld build/kernel_entry.o build/isr.o build/kernel.o build/idt.o build/pic.o build/keyboard.o build/screen.o build/fs.o -o build/kernel.bin
dd if=/dev/zero of=build/os.img bs=512 count=2880 2>/dev/null
dd if=build/boot.bin of=build/os.img bs=512 count=1 conv=notrunc 2>/dev/null
dd if=build/kernel.bin of=build/os.img bs=512 seek=1 conv=notrunc 2>/dev/null
qemu-system-i386 -drive format=raw,file=build/os.img