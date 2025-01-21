#include "mem.h"

#define INT_GATE    0x8E00
#define TRAP_GATE   0xEF00

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

void init_idt_desc(u16, u32, u16, struct idtdesc*);
void init_idt(void);
void init_pic(void);

