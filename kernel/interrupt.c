#include "types.h"
#include "tty.h"
#include "io.h"
#include "keyboard.h"

void isr_default(void) {
    terminal_write("Default interrupt\n");
}

void isr_GP_ex(void) {
   terminal_write("GP Fault!\n");
   while(1);
}

void isr_clock(void) {
    static int tic = 0;
    static int sec = 0;
    tic++;
    if (tic % 100 == 0) {
        sec++;
	tic = 0;
	terminal_putchar('.');
    }
}

void isr_keyboard(void) {
    
	u8 i;
	static short lshift, rshift;
	do{
	    i = inb(0x64);
	    
	} while((i & 0x01)==0);

	    i = inb(0x60);
 	    i--;
	    if (i >= 0x80){
	    	    i=i-0x80;
		    switch (i) {
		    case 0x29:
			lshift = 0;
			break;
		    case 0x35:
		        rshift = 0;
     			break;
	
		}
	    }else{
	    	switch (i) {
			case 0x29:
			    lshift = 1;
		            break;
			case 0x35:
			    rshift = 1;
			    break;

			default:
			    terminal_putchar(kbdmap[i * 4 + (lshift || rshift)]);
			   
			
		}			    
	    	
	    }    



}
