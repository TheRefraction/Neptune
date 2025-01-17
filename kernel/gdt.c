#include "types.h"
#include "lib/string.h"

#define __KERNEL_GDT__
#include "gdt.h"

// Fonction pour initialiser un descripteur
void init_gdt_desc(u32 base, u32 limit, u8 access, u8 other, struct gdtdesc *desc) {
    desc->limit_low = (limit & 0xFFFF);
    desc->base_low = (base & 0xFFFF);
    desc->base_middle = (base & 0xFF0000) >> 16;
    desc->access = access;
    desc->limit_high = (limit & 0xF0000) >> 16;
    desc->other = (other & 0xF);
    desc->base_high = (base & 0xFF000000) >> 24;
}

void init_gdt(void) {
    default_tss.debug_flag = 0x00;
    default_tss.io_map = 0x00;
    default_tss.esp0 = 0x20000;
    default_tss.ss0 = 0x18;
    
    init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]); // NULL DESCRIPTOR
    
    init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]); // CODE 
    init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]); // DATA
    init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]); // STACK
  
    init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]); // USER CODe
    init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]); // USER DATA
    init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]); // USER STACK
    
  init_gdt_desc((u32) &default_tss, 0x67, 0xE9, 0x00, &kgdt[7]);

    kgdtr.limit = 8 * GDTSIZE;
    kgdtr.base = GDTBASE;

    memcpy((char*) kgdtr.base, (char*) kgdt, kgdtr.limit);

    // Load the new GDT into the CPU
    asm("lgdtl (kgdtr)");

    // Perform a long jump to reset all registers to new GDT
    asm("movw $0x10, %ax \n \
	movw %ax, %ds \n \
	movw %ax, %es \n \
	movw %ax, %fs \n \
	movw %ax, %gs \n \
	ljmp $0x08, $next \n \
	next: \n");
}
