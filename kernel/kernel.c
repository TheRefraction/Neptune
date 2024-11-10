#include "tty.h"
#include "io.h"

void kernel_main() {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");

	/*
	 *
	init_idt();
	terminal_write("IDT has been loaded.\n");

	init_pic();
	terminal_write("PIC has been configured.\n");

	init_gdt();

	asm("movw $0x18, %ax \n \
		movw %ax, %ss \n \
		movl $0x20000, %esp");

	terminal_write("New GDT has been loaded.\n");
	
	sti;
	terminal_write("Interrupts are enabled.\n");*/

	while(1);
}
