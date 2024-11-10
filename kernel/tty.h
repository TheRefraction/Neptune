#ifndef __KERNEL_TTY__
#define __KERNEL_TTY__

#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);
void terminal_putchar(char);
void terminal_write(const char*);
void terminal_movecursor(int, int);
void terminal_scroll(unsigned int);
void terminal_setcolor(uint8_t);

void enable_cursor(uint8_t, uint8_t);
void disable_cursor();
void update_cursor(uint16_t, uint16_t);
uint16_t getpos_cursor(void);

#endif
