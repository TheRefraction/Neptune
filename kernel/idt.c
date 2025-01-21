#include "io.h"
#include "lib.h"
#include "mem.h"

#define __KERNEL_IDT__
#include "idt.h"

void _asm_default_irq(void);
void _asm_irq_0(void);
void _asm_irq_1(void);

void _asm_ex_GP(void);
void _asm_ex_PF(void);

void _asm_syscalls(void);

void init_idt_desc(u16 select, u32 offset, u16 type, struct idtdesc* desc) {
  desc->offset_low = (offset & 0xFFFF);
  desc->select = select;
  desc->type = type;
  desc->offset_high = (offset & 0xFFFF0000) >> 16;
}

void init_idt(void) {
  for (int i = 0; i < IDT_SIZE; i++) {
    init_idt_desc(0x08, (u32) _asm_default_irq, INT_GATE, &kidt[i]);
  }

  init_idt_desc(0x08,   (u32) _asm_ex_GP,     INT_GATE,     &kidt[13]); // General Protection Fault
  init_idt_desc(0x08,   (u32) _asm_ex_PF,     INT_GATE,     &kidt[14]); // Page Fault

  init_idt_desc(0x08,   (u32) _asm_irq_0,     INT_GATE,     &kidt[32]); // Clock INT
  init_idt_desc(0x08,   (u32) _asm_irq_1,     INT_GATE,     &kidt[33]); // Keyboard INT
	
  init_idt_desc(0x08,   (u32) _asm_syscalls,  TRAP_GATE,    &kidt[48]); // System calls (0x30)

  kidtr.limit = 8 * IDT_SIZE;
  kidtr.base = IDT_BASE;

  memcpy((char*) kidtr.base, (char*) kidt, kidtr.limit);

  asm("lidtl (kidtr)");
}

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
