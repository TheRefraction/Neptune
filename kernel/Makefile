OBJ=kernel
CC=$(PREFIX)/bin/$(TARGET)-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: $(OBJ)

kernel: kernel_entry.o kernel.o tty.o 
	@echo "Building N/OS Kernel"
	$(PREFIX)/bin/$(TARGET)-ld --oformat binary -Ttext 1000 $^ -o $@.bin

kernel_entry.o: kernel_entry.s
	@echo "Building Kernel Entry Point"
	nasm -f elf $^ -o $@

.o: .c
	$(CC) -c $^

clean:
	rm -f $(OBJ).bin *.o
