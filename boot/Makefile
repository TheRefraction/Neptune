OBJ=boot

all: $(OBJ)

boot: boot.asm
	nasm -f bin $^ -o $@.bin ;

clean:
	rm -f *.bin
