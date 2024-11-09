#!/bin/bash
rm -rf bin
mkdir bin

source ./set_var.sh 

echo "Building bootloader"
nasm -f bin -o "bin/boot.bin" "boot/boot.asm"

echo "Building Neptune/OS kernel"
nasm -f elf "kernel/kernel_entry.s" -o "bin/kernel_entry.o"
$TARGET-gcc -c "kernel/kernel.c" -o "bin/kernel.o" -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo "Linking kernel"
$TARGET-ld --oformat binary -o "bin/kernel.bin" -Ttext 1000 bin/kernel_entry.o bin/kernel.o

echo "Building image"
cat bin/boot.bin bin/kernel.o /dev/zero | dd of=floppyA bs=512 count=2880

echo "Done!"
