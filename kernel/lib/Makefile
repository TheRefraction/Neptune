OBJ=lib
CC=$(PREFIX)/bin/$(TARGET)-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra

all: $(OBJ)

lib: lib.o
	mv $^ ..

lib.o: string.c
	$(CC) $(CFLAGS) -c $^ -o $@
