#ifndef __KERNEL_TTY__
#define __KERNEL_TTY__

#include "types.h"

#define SCREEN_CTRL 0x3D4
#define SCREEN_DATA 0x3D5

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define VGA_MEMORY_LIMIT 0xB8FA0
#define VGA_MEMORY_SIZE 0xFA0

void terminal_initialize(void);
void terminal_putchar(char);
void terminal_write(char*);
void terminal_movecursor(int, int);
void terminal_scroll(u32);
void terminal_setcolor(u8);
void terminal_dump(u8*, u32);

void enable_cursor(u8, u8);
void disable_cursor(void);
void update_cursor(u16, u16);
u16 getpos_cursor(void);

#endif
