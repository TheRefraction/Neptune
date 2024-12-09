#include "types.h"
#include "gdt.h"
#include "tty.h"

void update();

void kernel_main() {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");

	init_gdt();

	asm("movw $0x18, %ax \n \
		movw %ax, %ss \n \
		movl $0x20000, %esp");

	update();
}

void update() {
    terminal_write("New GDT loaded.\n");

    while(1);
}
