#include "types.h"
#include "lib.h"
#include "gdt.h"
#include "tty.h"
#include "io.h"
#include "idt.h"
#include "mem.h"
#include "process.h"

void kernel_main(void);

void kernel_start(void) {
	terminal_initialize();
	terminal_write("Codename Neptune.\n");
  
	kernel_main();
}

void task1(void) {
  char *msg = (char*) 0x40001000;
  unsigned int i;

  msg[0] = 'T';
  msg[1] = 'a';
  msg[2] = 's';
  msg[3] = 'k';
  msg[4] = '1';
  msg[5] = '\n';
  msg[6] = 0;

  while(1) {
    // Call syscall n°1 (eax) and prints the string loaded in ebx
    asm("mov %0, %%ebx; mov $0x01, %%eax; int $0x30" :: "m" (msg));
    for (i = 0; i < 1000000; i++);
  }

  return;
}

void task2(void) {
  char *msg = (char*) 0x40001000;
  unsigned int i;

  msg[0] = 'T';
  msg[1] = 'a';
  msg[2] = 's';
  msg[3] = 'k';
  msg[4] = '2';
  msg[5] = '\n';
  msg[6] = 0;

  while(1) {
    // Call syscall n°1 (eax) and prints the string loaded in ebx
    asm("mov %0, %%ebx; mov $0x01, %%eax; int $0x30" :: "m" (msg));
    for (i = 0; i < 1000000; i++);
  }

  return;
}

void kernel_main(void) {
  cli;

	init_gdt();

	// Initialize the stack with segment descriptor 0x18 (offset 24 in GDTR -> Stack GDT) 
	asm("movw $0x18, %%ax; movw %%ax, %%ss; movl %0, %%esp":: "i"(KRNL_STACK));

  u32* base_upper_mem = (u32*) 0x7000;
  u32 mem_size = (*base_upper_mem + *(base_upper_mem + 1) - 1) / 1024; //KiB
  
  printf("Detected RAM: %uKiB.\n", mem_size + 1);

  terminal_write("GDT loaded.\n");

  init_idt();
	terminal_write("IDT loaded.\n");

	init_pic();
	terminal_write("PIC initialized.\n");

  // Load the Task Register (TR) with segment descriptor 0x38 (offset 56 in GDTR -> Default TSS)
	asm("movw $0x38, %ax; ltr %ax");
	terminal_write("Task Register loaded.\n");

  init_memory(mem_size);
  terminal_write("Paging enabled.\n");

  // Load tasks to their respective physical address
  load_task((char*) &task1, 0x2000);
  load_task((char*) &task2, 0x2000);
  terminal_write("Tasks loaded.\n");

  terminal_write("Interrupts enabled.\n");
  sti;

  while(1);
}
