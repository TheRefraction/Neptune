#include "types.h"
#include "gdt.h"
#include "tty.h"
#include "io.h"
#include "idt.h"
#include "paging.h"
#include "process.h"

#include "lib/string.h"

void init_pic(void);

void kernel_main(void);

void kernel_start(void) {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");	

	init_gdt();	

	// Initialize the stack with segment descriptor 0x18 (offset 24 in GDTR -> Stack GDT) 
	// and set the top of the stack at address 0x20000
	asm("movw $0x18, %ax \n \
		movw %ax, %ss \n \
		movl $0x20000, %esp"); 

	kernel_main();
}

void task1(void) {
  char *msg = (char*) 0x40000100;
  msg[0] = 'T';
  msg[1] = 'a';
  msg[2] = 's';
  msg[3] = 'k';
  msg[4] = '1';
  msg[5] = '\n';
  msg[6] = 0;

  int i;

  while(1) {
  // Call syscall n°1 (eax) and prints the string loaded in ebx
    asm("mov %0, %%ebx \n \
      mov $0x01, %%eax \n \
	    int $0x30" :: "m" (msg));
    for (i = 0; i < 10000000; i++);
  }
  
  return;
}

void task2(void) {
  char *msg = (char*) 0x40000100;
  msg[0] = 'T';
  msg[1] = 'a';
  msg[2] = 's';
  msg[3] = 'k';
  msg[4] = '2';
  msg[5] = '\n';
  msg[6] = 0;

  int i;

  while(1) {
  // Call syscall n°1 (eax) and prints the string loaded in ebx
    asm("mov %0, %%ebx \n \
      mov $0x01, %%eax \n \
	    int $0x30" :: "m" (msg));
    for (i = 0; i < 10000000; i++);
  }
  
  return;
}

void kernel_main(void) {
  terminal_write("GDT loaded.\n");

  init_idt();
	terminal_write("IDT loaded.\n");

	init_pic();
	terminal_write("PIC initialized.\n");

  // Load the Task Register (TR) with segment descriptor 0x38 (offset 56 in GDTR -> Default TSS)
	asm("movw $0x38, %ax \n \
			ltr %ax");
	terminal_write("Task Register loaded.\n");

  init_paging();
  terminal_write("Paging enabled.\n");

  // Load tasks to their respective physical address
  load_task((u32*) 0x100000, (u32*) &task1, 0x2000); 
  load_task((u32*) 0x200000, (u32*) &task2, 0x2000);
  terminal_write("Tasks loaded.\n");

  terminal_write("Interrupts enabled.\n");
  sti;

  while(1);

  hlt;
}
