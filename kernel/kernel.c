#include "types.h"
#include "gdt.h"
#include "tty.h"
#include "io.h"
#include "idt.h"

void init_pic(void);

void kernel_main(void);

void kernel_start(void) {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");

	init_idt();
	terminal_write("IDT loaded.\n");

	init_pic();
	terminal_write("PIC initialized.\n");

	init_gdt();

	asm("movw $0x18, %ax \n \
		movw %ax, %ss \n \
		movl $0x20000, %esp");

	kernel_main();
}

void kernel_main(void) {
    terminal_write("New GDT loaded.\n");

    sti;
    terminal_write("Interrupts enabled.\n");

    while(1);
}
