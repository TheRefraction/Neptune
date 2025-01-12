#include "types.h"
#include "tty.h"
#include "io.h"

void do_syscalls(u32 id) {
  char* str;
  int i;

  switch(id) {
    case 1: { // Display text
      asm("mov %%ebx, %0": "=m"(str): );
	    for (i = 0; i < 100000; i++);
      cli;
      terminal_write(str);
      sti;
	    break;
	  }
	  default:
	    terminal_write("Unknown system call!\n");
  }

  return;
}
