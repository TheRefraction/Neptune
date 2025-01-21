OBJ=krnl32
CC=$(PREFIX)/bin/$(TARGET)-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra
LD=$(PREFIX)/bin/$(TARGET)-ld
LDFLAGS=--oformat binary -Ttext 100000 
NASM=nasm -f elf

all: $(OBJ)

krnl32: kernel_entry.o kernel.o tty.o lib.o gdt.o idt.o interrupt.o int.o pic.o syscalls.o mem.o process.o printf.o schedule.o sched.o com.o malloc.o
	@echo "Building N/OS Kernel"
	$(LD) $(LDFLAGS) $^ -o $@.bin

kernel_entry.o: kernel_entry.s
	@echo "Building Kernel Entry Point"
	$(NASM) -o $@ $^

int.o: int.asm
	$(NASM) -o $@ $^

sched.o: sched.asm
	$(NASM) -o $@ $^

.o: .c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(OBJ).bin *.o *.a

indent: 
	indent -kr -i8 -ts8 *.c *.h
