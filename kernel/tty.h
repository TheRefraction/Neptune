#ifndef __KERNEL_TTY__
#define __KERNEL_TTY__

#include "types.h"

void terminal_initialize(void);
void terminal_putchar(char);
void terminal_write(const char*);
void terminal_movecursor(int, int);
void terminal_scroll(u32);
void terminal_setcolor(u8);

void enable_cursor(u8, u8);
void disable_cursor(void);
void update_cursor(u16, u16);
u16 getpos_cursor(void);

#endif
