#include "types.h"
#include "tty.h"

void isr_default(void) {
    terminal_write("Default interrupt\n");
}

void isr_clock(void) {
    static int tic = 0;
    static int sec = 0;
    tic++;
    if (tic % 100 == 0) {
        sec++;
	tic = 0;
	terminal_write("Clock\n");
    }
}

void isr_keyboard(void) {
    terminal_write("Keyboard interrupt\n");
}
