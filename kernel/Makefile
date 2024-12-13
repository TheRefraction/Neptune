OBJ=kernel
CC=$(PREFIX)/bin/$(TARGET)-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LD=$(PREFIX)/bin/$(TARGET)-ld
LDFLAGS=--oformat binary -Ttext 2000
NASM=nasm -f elf

all: $(OBJ)

kernel: kernel_entry.o kernel.o tty.o com.o lib.o gdt.o idt.o interrupt.o int.o pic.o syscalls.o 
	@echo "Building N/OS Kernel"
	$(LD) $(LDFLAGS) $^ -o $@.bin

kernel_entry.o: kernel_entry.s
	@echo "Building Kernel Entry Point"
	$(NASM) $^ -o $@

int.o: int.asm
	$(NASM) $^ -o $@

lib.o: 
	@echo "Building N/OS LibK"
	@make -C lib

.o: .c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(OBJ).bin *.o
