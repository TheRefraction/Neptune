OBJ=libc
CC=$(PREFIX)/bin/$(TARGET)-gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: $(OBJ)

libc: string.o
	@echo "Building N/OS C Library"

.o: .c
	$(CC) -c $^

clean:
	rm -f $(OBJ).bin *.o
