#include <stdint.h>

#define IDT_ENTRIES 256

// Structure pour une entrée de l'IDT (8 octets)
struct idt_entry {
    uint16_t offset_low;    // Bits 0-15 de l'adresse ISR
    uint16_t selector;      // Sélecteur de segment (CS)
    uint8_t  zero;          // Champ réservé (doit être 0)
    uint8_t  type_attr;     // Type et attributs
    uint16_t offset_high;   // Bits 16-31 de l'adresse ISR
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;         // Taille de l'IDT - 1
    uint32_t base;          // Adresse de base de l'IDT
} __attribute__((packed));

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_ptr;

void idt_set_entry(int num, uint32_t base, uint16_t selector, uint8_t type_attr) {
    idt[num].offset_low = base & 0xFFFF;        // Partie basse de l'adresse
    idt[num].selector = selector;              // Sélecteur de segment
    idt[num].zero = 0;                         // Champ réservé à 0
    idt[num].type_attr = type_attr;            // Type et attributs
    idt[num].offset_high = (base >> 16) & 0xFFFF; // Partie haute de l'adresse
}
