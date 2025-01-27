#include "io.h"
#include "lib.h"
#include "mem.h"

#define __KERNEL_IDT__
#include "idt.h"

void _asm_default_irq(void);
void _asm_irq_0(void);
void _asm_irq_1(void);

void _asm_ex_DE(void);
void _asm_ex_DB(void);
void _asm_ex_NMI(void);
void _asm_ex_BP(void);
void _asm_ex_OF(void);
void _asm_ex_BR(void);
void _asm_ex_UD(void);
void _asm_ex_NM(void);
void _asm_ex_DF(void);
void _asm_ex_TS(void);
void _asm_ex_NP(void);
void _asm_ex_SS(void);
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

  init_idt_desc(0x08,   (u32) _asm_ex_DE,     INT_GATE,     &kidt[0]);  // Division by zero
  init_idt_desc(0x08,   (u32) _asm_ex_DB,     INT_GATE,     &kidt[1]);  // Debug
  init_idt_desc(0x08,   (u32) _asm_ex_NMI,    INT_GATE,     &kidt[2]);  // Non Maskable Interrupt
  init_idt_desc(0x08,   (u32) _asm_ex_BP,     INT_GATE,     &kidt[3]);  // Breakpoint
  init_idt_desc(0x08,   (u32) _asm_ex_OF,     INT_GATE,     &kidt[4]);  // Overflow
  init_idt_desc(0x08,   (u32) _asm_ex_BR,     INT_GATE,     &kidt[5]);  // Bound Range Exceeded
  init_idt_desc(0x08,   (u32) _asm_ex_UD,     INT_GATE,     &kidt[6]);  // Invalid Opcode
  init_idt_desc(0x08,   (u32) _asm_ex_NM,     INT_GATE,     &kidt[7]);  // Device Not Available
  init_idt_desc(0x08,   (u32) _asm_ex_DF,     INT_GATE,     &kidt[8]);  // Double Fault
  //init_idt_desc(0x08,   (u32) _asm_default_irq, INT_GATE,     &kidt[9]);  // Coprocessor Segment Overrun
  init_idt_desc(0x08,   (u32) _asm_ex_TS,     INT_GATE,     &kidt[10]); // Invalid TSS
  init_idt_desc(0x08,   (u32) _asm_ex_NP,     INT_GATE,     &kidt[11]); // Segment Not Present
  init_idt_desc(0x08,   (u32) _asm_ex_SS,     INT_GATE,     &kidt[12]); // Stack-Segment Fault
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
