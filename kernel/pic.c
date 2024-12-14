#include "io.h"

void init_pic(void) {
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, 0x20); // Vector -> 32
    outb(0xA1, 0x70); // Vector -> 96

    // ICW3
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // INT MASK
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}