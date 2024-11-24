#include "vga.h"
#include "io.h"
#include "tty.h"


// macro for code lisibility
#define SCREEN_CTRL 0x3D4
#define SCREEN_DATA 0x3D5

// TODO: Move these variables inside tty.h
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static const uint16_t* VGA_MEMORY = (uint16_t*) 0xB8000;
static const uint16_t* VGA_MEMORY_LIMIT = (uint16_t*) 0xB8FA0;

static unsigned int terminal_row;
static unsigned int terminal_col;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void enable_cursor(uint8_t start, uint8_t end) {
	outb(SCREEN_CTRL, 0x0A);
	outb(SCREEN_DATA, (inb(SCREEN_DATA) & 0xC0) | start);

	outb(SCREEN_CTRL, 0x0B);
	outb(SCREEN_DATA, (inb(SCREEN_DATA) & 0xE0) | end);
}

void disable_cursor() {
	outb(SCREEN_CTRL, 0x0A);
	outb(SCREEN_DATA, 0x20);
}

void update_cursor(uint16_t x, uint16_t y) {
	uint16_t pos = y * VGA_WIDTH + x;

	outb(SCREEN_CTRL, 0x0F);
	outb(SCREEN_DATA, (uint8_t) (pos & 0xFF));

	outb(SCREEN_CTRL, 0x0E);
	outb(SCREEN_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t getpos_cursor(void) {
	uint16_t pos = 0;

	outb(SCREEN_CTRL, 0x0F);
	pos |= inb(SCREEN_DATA);

	outb(SCREEN_CTRL, 0x0E);
	pos |= ((uint16_t) inb(SCREEN_DATA)) << 8;

	return pos;
}

void terminal_putentryat(char c, uint8_t color, unsigned int x, unsigned int y) {
	terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void terminal_initialize(void) {
	enable_cursor(13, 15);
	update_cursor(0, 0);

	terminal_row = 0;
	terminal_col = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
	terminal_buffer = VGA_MEMORY;

	for (unsigned int y = 0; y < VGA_HEIGHT; y++) {
		for (unsigned int x = 0; x < VGA_WIDTH; x++) {
			terminal_putentryat(' ', terminal_color, x, y);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_scroll(unsigned int line) {
	uint16_t* tmp = 0;

	for (uint16_t* video = VGA_MEMORY; video < VGA_MEMORY_LIMIT; video += 2) {
		tmp = video + line * 160;

		if (tmp < VGA_MEMORY_LIMIT) {
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

		for (unsigned int x = 0; x < VGA_WIDTH; x++) {
			terminal_buffer[24 * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
		}

		terminal_row = VGA_HEIGHT - 1;
	}

	update_cursor(terminal_col, terminal_row);
}

void terminal_write(const char* data) {
	unsigned int i = 0;
	
	while (data[i] != '\0') {
		terminal_putchar(data[i]);
		i++;
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
