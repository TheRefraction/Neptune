all: boot loader

boot: boot.asm
	nasm -f bin $^ -o BOOT.BIN

loader: loader.asm
	nasm -f bin $^ -o KRNLLDR.SYS

clean:
	rm -f *.bin *.sys *.BIN *.SYS
