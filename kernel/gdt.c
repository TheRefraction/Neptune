#include <stdint.h>

/* descripteur gdt standard (8 octets)
 * limite basse 16 bits
 * base basse 16 bits
 * base moyenne 8 bits
 * acces 8 bits
 * limite haute et drapeaux (granularity) 8 bits 
 * base haute 8 bits
 */
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)); // pas de padding

struct gdt_ptr {
    uint16_t limit; // taille de la GDT - 1
    uint32_t base; // adresse de la GDT
} __attribute__((packed));


struct gdt_entry gdt[3];
struct gdt_ptr gdt_ptr;

// fonction pour initialiser un descripteur
void gdt_set_entry(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= granularity & 0xF0;
    gdt[num].access = access;
}
