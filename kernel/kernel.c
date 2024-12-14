#include "types.h"
#include "gdt.h"
#include "tty.h"
#include "io.h"
#include "idt.h"
#include "lib/string.h"

extern struct tss default_tss;

void init_pic(void);
void init_paging(void);

void kernel_main(void);

void kernel_start(void) {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");

	init_idt();
	terminal_write("IDT loaded.\n");

	init_pic();
	terminal_write("PIC initialized.\n");

	init_gdt();
	terminal_write("GDT loaded.\n");

	// Load the Task Register (TR) with segment descriptor 0x38 (offset 56 in GDTR -> Default TSS)
	asm("movw $0x38, %ax \n \
			ltr %ax");
	terminal_write("Task Register loaded.\n");

	// Initialize the stack with segment descriptor 0x18 (offset 24 in GDTR -> Stack GDT) 
	// and set the top of the stack at address 0x20000
	asm("movw $0x18, %ax \n \
		movw %ax, %ss \n \
		movl $0x20000, %esp"); 

	kernel_main();
}

// UNUSED FOR NOW
void task1(void) {
    char *msg = (char*) 0x100;
    msg[0] = 'T';
    msg[1] = 'a';
    msg[2] = 's';
    msg[3] = 'k';
    msg[4] = 0;

    // Call syscall n°1 (eax) and prints the string loaded in ebx
    asm("mov %0, %%ebx \n \
         mov $0x01, %%eax \n \
	 int $0x30" :: "m" (msg));

    while(1);
    return;
}

void kernel_main(void) {
    //memcpy((char*) 0x30000, (char*) &task1, 100);
    
    //terminal_write("Switching to user task\n");
    /*
     * Disable interrupts
     * Push UStack Selector 0x30 + ring 3 -> 0x33
     * Push the top of the stack (defines a memory space of 4Kio from base 0x20000 -> 0x30000)
     * Push EFLAGS register and disable NT flag (on I386 CPUs, disable hardware task switching) and enable IF flag (allows Interrupts in user mode)
     * Push User Code selector(0x20 + ring 3 -> 0x23) and pointer (0x0)
     * Set default_tss.esp0 (kernel stack) to 0x20000
     * Set data segment (DS) to User Data selector (0x28 + ring 3 -> 0x2B)
     * Pop return address (cs:eip), EFLAGS register and stack registers (ss and esp).
     */
    /*asm("cli \n \
         push $0x33 \n \
         push $0x30000 \n \
	 pushfl \n \
         popl %%eax \n \
	 orl $0x200, %%eax \n \
	 and $0xFFFFBFFF, %%eax \n \
	 push %%eax \n \
	 push $0x23 \n \
	 push $0x0 \n \
	 movl $0x20000, %0 \n \
	 movw $0x2B, %%ax \n \
	 movw %%ax, %%ds \n \
	 iret": "=m"(default_tss.esp0):);
   
    terminal_write("FATAL EXCEPTION! N/OS KERNEL HALTING!!\n");
    hlt;*/

    init_paging();
    terminal_write("Paging enabled.\n");

    sti;
    terminal_write("Interrupts enabled.\n");

    while(1);

    hlt;
}
