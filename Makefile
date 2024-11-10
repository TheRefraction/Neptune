OBJ=floppyA
TARGET=i686-elf
PREFIX=/opt/cross/bin

export TARGET
export PREFIX

all: $(OBJ)

boot_s:
	make -C boot

kernel_s:
	make -C kernel

floppyA: boot_s kernel_s
	cat boot/boot.bin kernel/kernel.bin /dev/zero | dd of=floppyA bs=512 count=2880

run: floppyA
	make -C bochs

clean:
	rm -f $(OBJ) *.o
	make -C boot clean
	make -C kernel clean
