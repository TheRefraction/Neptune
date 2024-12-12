#define GDTBASE 0x0
#define GDTSIZE 0xFF

/* descripteur gdt standard (8 octets)
 * limite basse 16 bits
 * base basse 16 bits
 * base moyenne 8 bits
 * acces 8 bits
 * limite haute et drapeaux (granularity) 8 bits 
 * base haute 8 bits
 */
struct gdtdesc {
    u16 limit_low; // 0_15
    u16 base_low; // 0_15
    u8 base_middle; // 16_23
    u8 access;
    u8 granularity : 4; // 16_19
    u8 other : 4; 
    u8 base_high; // 24_31
} __attribute__((packed)); // pas de padding

struct gdtr {
    u16 limit; // taille de la GDT - 1
    u32 base; // adresse de la GDT
} __attribute__((packed));

void init_gdt_desc(u32, u32, u8, u8, struct gdtdesc*);
void init_gdt(void);

#ifdef __KERNEL_GDT__
    struct gdtdesc kgdt[GDTSIZE];
    struct gdtr kgdtr;
#else
    extern struct gdtdesc kgdt[];
    extern struct gdtr kgdtr;
#endif
