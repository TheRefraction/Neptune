#include "types.h"

#define PAGING_FLAG 0x80000000 // 31st bit of CR0 register

#define PAGE_PRESENT 0x1       // Indique que la page est présente en mémoire
#define PAGE_RW 0x2            // Indique que la page est en lecture/écriture
#define PAGE_USER 0x4          // Indique que la page est accessible par les utilisateurs

#define PAGE_SIZE 0x1000         // Taille d'une page (4 Kio)
#define PAGE_MAX 0x10000  

#define USER_STACK 0xE0000000
#define USER_OFFSET 0x40000000

#ifdef __KERNEL_PAGING__
  u32* pd0;
  u32* pt0;
  u8 mem_bitmap[PAGE_MAX / 8]; // 1Gio
#else 
  extern u8 mem_bitmap[];
#endif

// Get Page number in the bitmap of an address
#define PAGE(addr)    (addr) >> 12

// Get each component of a linear 32-bit address (PD index (31-22) + PT index (21-12) + Page offset (11-0))
#define VADDR_PD_OFFSET(addr)   ((addr) & 0xFFC00000) >> 22
#define VADDR_PT_OFFSET(addr)   ((addr) & 0x003FF000) >> 12
#define VADDR_PG_OFFSET(addr)   (addr) & 0x00000FFF

// Use or release a page
#define set_page_frame_used(page)   mem_bitmap[((u32) page) / 8] |= (1 << (((u32) page) % 8))
#define release_page_frame(p_addr)  mem_bitmap[((u32) p_addr / PAGE_SIZE) / 8] &= (1 << (((u32) p_addr / PAGE_SIZE) % 8))

struct pd_entry {
  u32 present:1;
  u32 writable:1;
  u32 user:1;
  u32 pwt:1;
  u32 pcd:1;
  u32 accessed:1;
  u32 _garbage:1;
  u32 page_size:1;
  u32 global:1;
  u32 avail:3;
  u32 page_table_base:20;
} __attribute__ ((packed));

struct pt_entry {
  u32 present:1;
  u32 writable:1;
  u32 user:1;
  u32 pwt:1;
  u32 pcd:1;
  u32 accessed:1;
  u32 dirty:1;
  u32 pat:1;
  u32 global:1;
  u32 avail:3;
  u32 page_base:20;
} __attribute__ ((packed));

char* get_page_frame(void);
void init_paging(void);
u32* pd_create(u32*, u32);
