#include "types.h"
#include "io.h"
#include "lib/string.h"
#include "idt.h"

struct idtr kidtr;
struct idtdesc kidt[IDTSIZE];

void _asm_default_irq(void);
void _asm_irq_0(void);
void _asm_irq_1(void);

void _asm_ex_GP(void);

void _asm_syscalls(void);

void init_idt_desc(u16 select, u32 offset, u16 type, struct idtdesc* desc) {
    desc->offset_low = (offset & 0xFFFF);
    desc->select = select;
    desc->type = type;
    desc->offset_high = (offset & 0xFFFF0000) >> 16;
}

void init_idt(void) {
    int i;
    for (i = 0; i < IDTSIZE; i++) {
        init_idt_desc(0x08, (u32) _asm_default_irq, INTGATE, &kidt[i]);
    }

    init_idt_desc(0x08, (u32) _asm_ex_GP, INTGATE, &kidt[13]); // General Protection Fault

    init_idt_desc(0x08, (u32) _asm_irq_0, INTGATE, &kidt[32]); // Clock INT
    init_idt_desc(0x08, (u32) _asm_irq_1, INTGATE, &kidt[33]); // Keyboard INT
			
    init_idt_desc(0x08, (u32) _asm_syscalls, TRAPGATE, &kidt[48]); // System calls (0x30)

    kidtr.limit = IDTSIZE * 8;
    kidtr.base = IDTBASE;

    memcpy((char*) kidtr.base, (char*) kidt, kidtr.limit);

    asm("lidtl (kidtr)");
}
