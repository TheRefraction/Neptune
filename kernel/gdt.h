#define GDTBASE 0x0
#define GDTSIZE 0xFF

/* descripteur gdt standard (8 octets)
 * limite basse 16 bits
 * base basse 16 bits
 * base moyenne 8 bits
 * acces 8 bits
 * limite haute  8 bits
 * autres drapeaux (granularité, taille des instructions, ...) 8 bits 
 * base haute 8 bits
 */
struct gdtdesc {
    u16 limit_low; // 0_15
    u16 base_low; // 0_15
    u8 base_middle; // 16_23
    u8 access;
    u8 limit_high : 4; // 16_19 Un demi-octet
    u8 other : 4; // Un demi-octet
    u8 base_high; // 24_31
} __attribute__((packed)); // pas de padding

struct gdtr {
    u16 limit; // taille de la GDT - 1
    u32 base; // adresse de la GDT
} __attribute__((packed));

void init_gdt_desc(u32, u32, u8, u8, struct gdtdesc*);
void init_gdt(void);

struct tss {
    u16 previous_task, __previous_task_unused;
    u32 esp0;
    u16 ss0, __ss0_unused;
    u32 esp1;
    u16 ss1, __ss1_unused;
    u32 esp2;
    u16 ss2, __ss2_unused;
    u32 cr3;
    u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u16 es, __es_unused;
    u16 cs, __cs_unused;
    u16 ss, __ss_unused;
    u16 ds, __ds_unused;
    u16 fs, __fs_unused;
    u16 gs, __gs_unused;
    u16 ldt_selector, __ldt_sel_unused;
    u16 debug_flag, io_map;
} __attribute__((packed));

#ifdef __KERNEL_GDT__
  struct gdtdesc kgdt[GDTSIZE];
  struct gdtr kgdtr;
  struct tss default_tss;
#else
  extern struct gdtdesc kgdt[];
  extern struct gdtr kgdtr;
  extern struct tss default_tss;
#endif
