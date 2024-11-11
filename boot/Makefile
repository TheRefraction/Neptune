all: boot loader
	cat boot.bin loader.bin > bootloader.bin

boot: boot.asm
	nasm -f bin $^ -o $@.bin

loader: loader.asm
	nasm -f bin $^ -o $@.bin

clean:
	rm -f *.bin
