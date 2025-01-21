#include "types.h"
#include "tty.h"
#include "io.h"
#include "keyboard.h"
#include "lib.h"
#include "process.h"
#include "schedule.h"

void isr_default(void) {
  printf("Default interrupt\n");
}

void isr_GP_ex(void) {
  printf("EX0x0D - General Protection Fault!\n");
  hlt;
}

void isr_PF_ex(void) {
  u32 addr;
  u32 eip;

  asm("movl 60(%%ebp), %%eax; \
	  mov %%eax, %0; \
	  mov %%cr2, %%eax; \
	  mov %%eax, %1" : "=m"(eip), "=m"(addr) : );

  printf("EX0x0E - Page Fault!\nEIP: %p\nCR2: %p", eip, addr);
  hlt;
}

void isr_clock(void) {
  static int tic = 0;
  static int sec = 0;

  tic++;
  if (tic % 100 == 0) {
    sec++;
	  tic = 0;
  }
  schedule();
}

void isr_keyboard(void) {
  u8 i;
  static u8 lshift, rshift, ctrl, alt;

  do {
	  // Recover keyboard status
    i = inb(0x64);
  } while((i & 0x01) == 0); // If keyboard buffer ready to be sent

  i = inb(0x60);
  i--;

  if (i >= 0x80){
    i -= 0x80;
	  switch (i) {
	    case 0x1C:
	      ctrl = 0;
	      break;
	    case 0x29:
	      lshift = 0;
		    break;
	    case 0x35:
	      rshift = 0;
        break;
	    case 0x37:
		    alt = 0;
		    break;
    }
  } else { // Pressed key
    switch (i) {
	    case 0x1C:
	      ctrl = 1;
		    break;
      case 0x29:
	      lshift = 1;
		    break;
	    case 0x35:
	      rshift = 1;
	      break;
	    case 0x37:
		    alt = 1;
		    break;
      default:
	      terminal_putchar(kbdmap[i * 4 + (lshift || rshift)]);
    }
  }
}
