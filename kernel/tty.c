#include "vga.h"
#include "io.h"

#include "tty.h"

void printf(char *s, ...);

static u32 terminal_row;
static u32 terminal_col;
static u8 terminal_color;
static u16* terminal_buffer;

void enable_cursor(u8 start, u8 end) {
	outb(SCREEN_CTRL, 0x0A);
	outb(SCREEN_DATA, (inb(SCREEN_DATA) & 0xC0) | start);

	outb(SCREEN_CTRL, 0x0B);
	outb(SCREEN_DATA, (inb(SCREEN_DATA) & 0xE0) | end);
}

void disable_cursor(void) {
	outb(SCREEN_CTRL, 0x0A);
	outb(SCREEN_DATA, 0x20);
}

void update_cursor(u16 x, u16 y) {
	u16 pos = y * VGA_WIDTH + x;

	outb(SCREEN_CTRL, 0x0F);
	outb(SCREEN_DATA, (u8) (pos & 0xFF));

	outb(SCREEN_CTRL, 0x0E);
	outb(SCREEN_DATA, (u8) ((pos >> 8) & 0xFF));
}

u16 getpos_cursor(void) {
	u16 pos = 0;

	outb(SCREEN_CTRL, 0x0F);
	pos |= inb(SCREEN_DATA);

	outb(SCREEN_CTRL, 0x0E);
	pos |= ((u16) inb(SCREEN_DATA)) << 8;

	return pos;
}

void terminal_putentryat(char c, u8 color, u32 x, u32 y) {
	terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void terminal_clear(void) {
	update_cursor(0, 0);

	terminal_row = 0;
	terminal_col = 0;
	for (u32 y = 0; y < VGA_HEIGHT; y++) {
		for (u32 x = 0; x < VGA_WIDTH; x++) {
			terminal_putentryat(' ', terminal_color, x, y);
		}
	}
}

void terminal_initialize(void) {
	enable_cursor(13, 15);
	
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
	terminal_buffer = (u16*) VGA_MEMORY;

	terminal_clear();
}

void terminal_setcolor(u8 color) {
	terminal_color = color;
}

void terminal_panic(char *data) {
	u32 *pa, cr0, cr2, cr3, cr4;
	asm("mov %%ebp, %0; \
		mov %%cr0, %%eax; mov %%eax, %1; \
		mov %%cr2, %%eax; mov %%eax, %2; \
		mov %%cr3, %%eax; mov %%eax, %3; \
		mov %%cr4, %%eax; mov %%eax, %4; ": "=m"(pa), "=m"(cr0), "=m"(cr2), "=m"(cr3), "=m"(cr4):);

	terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED));
	terminal_clear();

	terminal_write("N/OS Fatal Error\n");
	terminal_write(data);
	terminal_putchar('\n');
	terminal_putchar('\n');

	printf("eax: %p ecx: %p edx: %p ebx: %p\n", pa[12], pa[11], pa[10], pa[9]);
	printf(" ds: %p esi: %p edi: %p\n", pa[4], pa[6], pa[5]);
	printf(" ss: %p ebp: %p esp: %p\n", pa[17], pa[7], pa[16]);
	printf(" cs: %p eip: %p\n", pa[14], pa[13]);
	printf("cr0: %p cr2: %p cr3: %p cr4: %p\n", cr0, cr2, cr3, cr4);

	disable_cursor();
}

void terminal_scroll(u32 line) {
	u8 *video;
	u8 *tmp;

	for (video = (u8*) VGA_MEMORY; video < (u8*) VGA_MEMORY_LIMIT; video += 2) {
		tmp = (u8*) (video + line * 160);

		if (tmp < (u8*) VGA_MEMORY_LIMIT) {
			*video = *tmp;
			*(video + 1) = *(tmp + 1);
		} else {
			*video = 0;
			*(video + 1) = 0x07;
		}
	}

	terminal_row -= line;
}

void terminal_putchar(char c) {
	switch(c) {
		case 10: {
			terminal_col = 0;
			terminal_row++;
			break;
		}
    case 9: {
      terminal_col += 8 - (terminal_col % 8);
      break;
    }
		case 13: {
			terminal_col = 0;
	    break;
		}
		default: {
			terminal_putentryat(c, terminal_color, terminal_col, terminal_row);
			terminal_col++;
		}
	}

	if (terminal_col >= VGA_WIDTH) {
		terminal_col = 0;
		terminal_row++;
	}

	if (terminal_row >= VGA_HEIGHT) {
		terminal_col = 0;
		terminal_scroll(1);

		for (u32 x = 0; x < VGA_WIDTH; x++) {
			terminal_buffer[24 * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
		}

		terminal_row = VGA_HEIGHT - 1;
	}

	update_cursor(terminal_col, terminal_row);
}

void terminal_write(char* data) {
	u32 i = 0;
	
	while (data[i] != '\0') {
		terminal_putchar(data[i]);
		i++;
	}
}

void terminal_dump(u8* addr, u32 n) {
    char c1, c2;
    char *tab = "0123456789ABCDEF";

    while (n--) {
        c1 = tab[(*addr & 0xF0) >> 4];
	c2 = tab[*addr & 0xF0];

	terminal_putchar(c1);
	terminal_putchar(c2);
	addr++;
    }
}

void terminal_movecursor(int x, int y) {
	int m_x = terminal_col + x;

	if (m_x < 0) {
		if (terminal_row > 0) {
			terminal_col = VGA_WIDTH - 1;
			terminal_row--;
		} else terminal_col = 0;
	} else if (m_x >= VGA_WIDTH) {
		if (terminal_row < VGA_WIDTH - 1) {
			terminal_col = 0;
			terminal_row++;
		} else terminal_col = VGA_WIDTH - 1;
	} else terminal_col += x;

	int m_y = terminal_row + y;

	if (m_y < 0) {
		terminal_row = 0;
	} else if (m_y >= VGA_HEIGHT) {
		terminal_row = VGA_HEIGHT - 1;
	} else terminal_row += y;
}
