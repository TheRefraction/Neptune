#include "types.h"
#include "tty.h"
#include "io.h"
#include "keyboard.h"

void isr_default(void) {
    terminal_write("Default interrupt\n");
}

void isr_GP_ex(void) {
   terminal_write("EX0x0D - General Protection Fault!\n");
   hlt;
}

void isr_PF_ex(void) {
    u32 addr;
    u32 eip;

    asm("movl 60(%%ebp), %%eax \n \
	 mov %%eax, %0 \n \
	 mov %%cr2, %%eax \n \
	 mov %%eax, %1" : "=m"(eip), "=m"(addr) : );

    terminal_write("EX0x0E - Page Fault!\n");
    terminal_dump((u8*) &addr, 4);
    terminal_dump((u8*) &eip, 4);

    hlt;
}

void isr_clock(void) {
  static int tic = 0;
  static int sec = 0;
  tic++;
  if (tic % 100 == 0) {
    sec++;
	  tic = 0;
    schedule();
  }
  //schedule();
}

void isr_keyboard(void) {
    u8 i;
    static short lshift, rshift, ctrl, alt;
    do{
	// Recover keyboard status
        i = inb(0x64); 
    } while((i & 0x01) == 0); // If keyboard buffer ready to be sent

    i = inb(0x60);
    i--;

    // Released key
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
