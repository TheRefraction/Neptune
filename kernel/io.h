#ifndef __KERNEL_IO__
#define __KERNEL_IO__

#define cli asm("cli"::)

#define sti asm("sti"::)

#define outb(port,value) \
	asm volatile ("outb %%al, %%dx" :: "d" (port), "a" (value));

#define inb(port) ({ \
	unsigned char _v; \
	asm volatile ("inb %%dx, %%al" : "=a" (_v) : "d" (port)); \
	_v; \
})

#endif
