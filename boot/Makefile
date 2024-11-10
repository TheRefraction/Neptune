OBJ=boot

all: $(OBJ)

boot: boot.asm
	@echo "Building N/OS Bootloader"
	nasm -f bin $^ -o $@.bin

clean:
	rm -f *.bin
