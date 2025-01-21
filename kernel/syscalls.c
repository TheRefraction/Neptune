#include "types.h"
#include "lib.h"
#include "io.h"

void do_syscalls(int id) {
  char* str;
  u32 *pa;
  int i;

  switch(id) {
    case 1: { // Display text
      asm("mov %%ebx, %0": "=m"(str):);
	    for (i = 0; i < 1000000; i++);
      
      cli;
      printf(str);
      sti;

	    break;
	  }
    case 2: { // Registers dump
      asm("mov %%ebp, %0": "=m"(pa):);

      printf("eax: %p ecx: %p edx: %p ebx: %p\n", pa[12], pa[11], pa[10], pa[9]);
      printf("ds: %p esi: %p edi: %p\n", pa[4], pa[6], pa[5]);
      printf("ss: %p ebp: %p esp: %p\n", pa[17], pa[7], pa[16]);
      printf("cs: %p eip: %p\n", pa[14], pa[13]);

      break;
    }
	  default:
	    printf("Unknown system call %d!\n", id);
  }

  return;
}
