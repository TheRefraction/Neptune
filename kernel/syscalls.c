#include "types.h"
#include "gdt.h"
#include "tty.h"

void do_syscalls(u32 id) {
    u16 ds_select;
    u32 ds_base;
    struct gdtdesc *ds;
    u8 *msg;

    switch(id) {
        case 1: { // Display text
            asm("mov 44(%%ebp), %%eax \n \
		 mov %%eax, %0 \n \
		 mov 24(%%ebp), %%ax \n \
		 mov %%ax, %1" : "=m"(msg), "=m"(ds_select) : );

	    ds = (struct gdtdesc*) (GDTBASE + (ds_select & 0xF8));
	    ds_base = ds->base_low + (ds->base_middle << 16) + (ds->base_high << 24);

	    terminal_write((char*) (ds_base + msg));

	    break;
	}
	default:
	    terminal_write("Unknown system call!\n");
    }
}
