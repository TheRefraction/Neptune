#include "types.h"
#include "tty.h"
#include "io.h"
#include "keyboard.h"
#include "lib.h"
#include "schedule.h"

void isr_default(void) {
  printf("Default interrupt\n");
}

void isr_DE_ex(void) {
  terminal_panic("EX0x00 - Division by zero!\n");
  hlt;
}

void isr_DB_ex(void) {
  terminal_panic("EX0x01 - Debug!\n");
  hlt;
}

void isr_NMI_ex(void) {
  terminal_panic("EX0x02 - Non Maskable Interrupt!\n");
  hlt;
}

void isr_BP_ex(void) {
  terminal_panic("EX0x03 - Breakpoint!\n");
  hlt;
}

void isr_OF_ex(void) {
  terminal_panic("EX0x04 - Overflow!\n");
  hlt;
}

void isr_BR_ex(void) {
  terminal_panic("EX0x05 - Bound Range Exceeded!\n");
  hlt;
}

void isr_UD_ex(void) {
  terminal_panic("EX0x06 - Invalid Opcode!\n");
  hlt;
}

void isr_NM_ex(void) {
  terminal_panic("EX0x07 - Device Not Available!\n");
  hlt;
}

void isr_DF_ex(void) {
  terminal_panic("EX0x08 - Double Fault!\n");
  hlt;
}

void isr_TS_ex(void) {
  terminal_panic("EX0x0A - Invalid TSS!\n");
  hlt;
}

void isr_NP_ex(void) {
  terminal_panic("EX0x0B - Segment Not Present!\n");
  hlt;
}

void isr_SS_ex(void) {
  terminal_panic("EX0x0C - Stack-Segment Fault!\n");
  hlt;
}

void isr_GP_ex(void) {
  terminal_panic("EX0x0D - General Protection Fault!\n");
  hlt;
}

void isr_PF_ex(void) {
  terminal_panic("EX0x0E - Page Fault!");
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
