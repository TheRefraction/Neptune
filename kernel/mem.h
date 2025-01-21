#include "types.h"

#define PAGING_FLAG       0x80000000 // 31st bit of CR0 register
#define PSE_FLAG          0x00000010 // 4th bit of CR4 register

#define PAGE_PRESENT      0x00000001
#define PAGE_WRITE        0x00000002
#define PAGE_USER         0x00000004
#define PAGE_4MB          0x00000080

#define PAGE_SIZE         0x1000       // Taille d'une page (4 Kio)
#define RAM_MAXSIZE       0x100000000  // 4Gio
#define RAM_MAXPAGE       0x100000

#define IDT_SIZE          0xFF
#define GDT_SIZE          0xFF

#define IDT_BASE          0x00000000
#define GDT_BASE          0x00000800

#define KRNL_PD           0x00001000
#define KRNL_STACK        0x0009FFF0
#define KRNL_BASE         0x00100000
#define KRNL_PG_HEAP      0x00800000
#define KRNL_PG_HEAP_LIM  0x10000000 
#define KRNL_HEAP         0x10000000 
#define KRNL_HEAP_LIM     0x40000000 

#define USER_OFFSET       0x40000000
#define USER_STACK        0xE0000000

// Get Page number in the bitmap of an address
#define PAGE(addr)              (addr) >> 12

// Get each component of a linear 32-bit address (PD index (31-22) + PT index (21-12) + Page offset (11-0))
#define VADDR_PD_OFFSET(addr)   ((addr) & 0xFFC00000) >> 22
#define VADDR_PT_OFFSET(addr)   ((addr) & 0x003FF000) >> 12
#define VADDR_PG_OFFSET(addr)   (addr) & 0x00000FFF

#ifndef __KERNEL_MEM_STRUCT__
#define __KERNEL_MEM_STRUCT__

  struct page {
    char *v_addr;
    char *p_addr;
  };

  struct page_list {
    struct page *page;
    struct page_list *next;
    struct page_list *prev;
  };

  struct page_directory {
    struct page *base;
    struct page_list *pt;
  };

  struct vm_area {
    char *vm_start;
    char *vm_end;
    struct vm_area *next;
    struct vm_area *prev;
  };

#endif

#ifdef __KERNEL_MEM__ 
  u32 *pd0        =       (u32*) KRNL_PD;
  char *pg0       =       (char*) 0; // Kernel page 0 (4Mio)
  char *pg1       =       (char*) 0x400000; // Kernel page 1 (4Mio)
  char *pg1_end   =       (char*) 0x800000;
  u8 mem_bitmap[RAM_MAXPAGE / 8];
  char *krnl_heap;
  struct vm_area *free_vm;
#else 
  extern u32 *pd0;
  extern u8 mem_bitmap[];
  extern char *krnl_heap;
  extern struct vm_area *free_vm;
#endif

// Use or release a page
#define set_page_frame_used(page)	mem_bitmap[((u32) page)/8] |= (1 << (((u32) page)%8))
#define release_page_frame(p_addr)	mem_bitmap[((u32) p_addr/PAGE_SIZE)/8] &= ~(1 << (((u32) p_addr/PAGE_SIZE)%8))

char *get_page_frame(void);

struct page *get_page_from_heap(void);
int release_page_from_heap(char *);

void init_memory(u32);

struct page_directory *pd_create(void);
int pd_destroy(struct page_directory *);

int pd0_add_page(char *, char *, int);

int pd_add_page(char *, char *, int, struct page_directory *);
int pd_remove_page(char *);

char *get_p_addr(char *);