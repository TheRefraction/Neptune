OBJ=kernel
CC=$(PREFIX)/$(TARGET)-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: $(OBJ)

kernel: kernel_entry.o kernel.o 
	$(PREFIX)/$(TARGET)-ld --oformat binary -Ttext 1000 $^ -o $@.bin

kernel_entry.o: kernel_entry.s
	nasm -f elf $^ -o $@

.o: .c
	$(CC) -c $^

clean:
	rm -f $(OBJ).bin *.o
