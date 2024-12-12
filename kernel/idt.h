#ifndef __KERNEL_IDT__
#define __KERNEL_IDT__

#define IDTBASE 0x800
#define IDTSIZE 0xFF

#define INTGATE 0x8E00

// 8 bytes
struct idtdesc {
    u16 offset_low; // 0_15
    u16 select;
    u16 type;
    u16 offset_high; // 16_31
} __attribute__ ((packed));

struct idtr {
    u16 limit;
    u32 base;
} __attribute__ ((packed));

void init_idt_desc(u16, u32, u16, struct idtdesc *);
void init_idt(void);

#endif
