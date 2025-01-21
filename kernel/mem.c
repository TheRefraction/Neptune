#include "types.h"
#include "lib.h"
#include "io.h"
#include "process.h"

#define __KERNEL_MEM__
#include "mem.h"

#include "malloc.h"

char* get_page_frame(void) {
  int byte, bit;
  int page = -1;

  for (byte = 0; byte < RAM_MAXPAGE / 8; byte++) {
    if (mem_bitmap[byte] != 0xFF) {
      for (bit = 0; bit < 8; bit++) {
        if (!(mem_bitmap[byte] & (1 << bit))) {
          page = 8 * byte + bit;
          // Set the found page to reserved/used
          set_page_frame_used(page);
          return (char *) (page * PAGE_SIZE);
        }
      }
    }
  }

  // No available page
  return (char *) -1;
}

struct page *get_page_from_heap(void) {
  struct page *pg;
  struct vm_area *p;
  char *v_addr, *p_addr;

  p_addr = get_page_frame();
  if (p_addr < 0) {
    printf("PANIC: get_page_from_heap(): no page frame available.\n");
    hlt;
  }

  if (free_vm->vm_start == free_vm->vm_end) {
    printf("PANIC: get_page_from_heap(): not memory left in page heap.\n");
    hlt;
  }

  v_addr = free_vm->vm_start;

  if (free_vm->vm_end - free_vm->vm_start == PAGE_SIZE) {
    if (free_vm->next) {
      p = free_vm;
      free_vm = free_vm->next;
      free_vm->prev = 0;

      free(p);
    }
  } else {
    free_vm->vm_start += PAGE_SIZE;
  }

  pd0_add_page(v_addr, p_addr, 0);

  pg = (struct page *) malloc(sizeof(struct page));
  pg->v_addr = v_addr;
  pg->p_addr = p_addr;

  return pg;
}

int release_page_from_heap(char *v_addr) {
  struct vm_area *p, *to_del, *new_vm_area;
  char *p_addr = get_p_addr(v_addr);

  if (p_addr) {
    release_page_frame(p_addr);
  } else {
    printf("WARNING: release_page_from_heap(): no page frame with v_addr %x\n", v_addr);
    return 1;
  }

  pd_remove_page(v_addr);

  p = free_vm;
  while (p->vm_start < v_addr && p->next) {
    p = p->next;
  }

  if (v_addr + PAGE_SIZE == p->vm_start) {
    p->vm_start = v_addr;

    if (p->prev && p->prev->vm_end == p->vm_start) {
      to_del = p->prev;
      p->vm_start = p->prev->vm_start;
      p->prev = p->prev->prev;

      if (p->prev) {
        p->prev->next = p;
      }

      free(to_del);
    }
  } else if (p->prev && p->prev->vm_end == v_addr) {
    p->prev->vm_end += PAGE_SIZE;
  } else if (v_addr + PAGE_SIZE < p->vm_start) {
    new_vm_area = (struct vm_area *) malloc(sizeof(struct vm_area));
    new_vm_area->vm_start = v_addr;
    new_vm_area->vm_end = v_addr + PAGE_SIZE;
    new_vm_area->prev = p->prev;
    new_vm_area->next = p;

    p->prev = new_vm_area;

    if (new_vm_area->prev) {
      new_vm_area->prev->next = new_vm_area;
    }
  } else {
    printf("PANIC: release_page_from_heap(): invalid linked list.\n");
    hlt;
  }

  return 0;
}

void init_memory(u32 high_mem) { 
  int pg, pg_limit;
  unsigned long i;

  // Total number of pages in memory
  pg_limit = (high_mem * 1024) / PAGE_SIZE;

  // Bitmap initialization
  for (pg = 0; pg < pg_limit / 8; pg++) {
    mem_bitmap[pg] = 0;
  }

  for (pg = pg_limit / 8; pg < RAM_MAXPAGE / 8; pg++) {
    mem_bitmap[pg] = 0xFF;
  }

  for (pg = PAGE(0x0); pg < PAGE((u32) pg1_end); pg++) {
    set_page_frame_used(pg);
  }

  pd0[0] = (u32) pg0 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB);
  pd0[1] = (u32) pg1 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB);
  for (i = 2; i < 1023; i++) {
    pd0[i] = ((u32) pg1 + i * PAGE_SIZE) | (PAGE_PRESENT | PAGE_WRITE);
  }
  pd0[1023] = ((u32) pd0 | (PAGE_PRESENT | PAGE_WRITE));

  /*
   * Passage de l'adresse du répertoire de pages au registre CR3
   * Mise à 1 du bit de pagination (n°31) dans le registre CR0
   */
  asm("mov %0, %%eax; \
    mov %%eax, %%cr3; \
    mov %%cr4, %%eax; \
    or %2, %%eax; \
    mov %%eax, %%cr4; \
	  mov %%cr0, %%eax; \
	  or %1, %%eax; \
	  mov %%eax, %%cr0" :: "m"(pd0), "i"(PAGING_FLAG), "i"(PSE_FLAG));

  // Initialize kernel heap 
  krnl_heap = (char*) KRNL_HEAP;
  sbrk(1);

  // Initialize free virtual addresses list
  free_vm = (struct vm_area *) malloc(sizeof(struct vm_area));
  free_vm->vm_start = (char *) KRNL_PG_HEAP;
  free_vm->vm_end = (char *) KRNL_PG_HEAP_LIM;
  free_vm->next = 0;
  free_vm->prev = 0;

  return;
}

struct page_directory *pd_create(void) {
  struct page_directory *pd;
  u32 *pdir;
  int i;

  pd = (struct page_directory *) malloc(sizeof(struct page_directory));
  pd->base = get_page_from_heap();

  pdir = (u32 *) pd->base->v_addr;
  // Kernel space 
  for (i = 0; i < 256; i++) {
    pdir[i] = pd0[i];
  }

  // User space
  for (i = 256; i < 1023; i++) {
    pdir[i] = 0;
  }

  pdir[1023] = ((u32) pd->base->p_addr | (PAGE_PRESENT | PAGE_WRITE));

  pd->pt = 0; 

  return pd;
}

int pd_destroy(struct page_directory *pd) {
  struct page_list *pgh, *oldpgh;

  // Free page for current directory
  release_page_from_heap(pd->base->v_addr);

  // Free pages from tables
  pgh = pd->pt;

  while (pgh) {
    release_page_from_heap(pgh->page->v_addr);
    oldpgh = pgh;

    pgh = pgh->next;
    free(oldpgh);
  }

  free(pd);

  return 1;
}

int pd0_add_page(char *v_addr, char *p_addr, int flags) {
  u32 *pde;
  u32 *pte;

  if (v_addr > (char *) USER_OFFSET) {
    printf("ERROR : pd0_add_page(): %p not in kernel space!\n", v_addr);
    return 0;
  }

  // Check if pages table is present 
  pde = (u32 *) (0xFFFFF000 | (((u32) v_addr & 0xFFC00000) >> 20));
  if ((*pde & PAGE_PRESENT) == 0) {
    printf("PANIC: pd0_add_page(): kernel page table not found for v_addr %p.\n", v_addr);
    hlt;
  }

  //Add page in table 
  pte = (u32 *) (0xFFC00000 | (((u32) v_addr & 0xFFFFF000) >> 10));
  *pte = ((u32) p_addr) | (PAGE_PRESENT | PAGE_WRITE | flags);

  return 0;
}

int pd_add_page(char *v_addr, char *p_addr, int flags, struct page_directory *pd) {
  u32 *pde;
  u32 *pte;
  u32 *pt;
  struct page *newpg;
  struct page_list *pglist;
  int i;

  pde = (u32 *) (0xFFFFF000 | (((u32) v_addr & 0xFFC00000) >> 20));

  // Create pages table if not present in pd 
  if((*pde & PAGE_PRESENT) == 0) {
    newpg = get_page_from_heap();

    pt = (u32 *) newpg->v_addr;
    for (i = 1; i < 1024; i++) {
      pt[i] = 0;
    }

    *pde = (u32) newpg->p_addr | (PAGE_PRESENT | PAGE_WRITE | flags);

    if (pd) {
      if (pd->pt) {
        pglist = (struct page_list *) malloc(sizeof(struct page_list));
        pglist->page = newpg;
        pglist->next = pd->pt;
        pglist->prev = 0;

        pd->pt->prev = pglist;
        pd->pt = pglist;
      } else {
        pd->pt = (struct page_list *) malloc(sizeof(struct page_list));
        pd->pt->page = newpg;
        pd->pt->next = 0;
        pd->pt->prev = 0;
      }
    }
  }

  pte = (u32 *) (0xFFC00000 | (((u32) v_addr & 0xFFFFF000) >> 10));
  *pte = ((u32) p_addr) | (PAGE_PRESENT | PAGE_WRITE | flags);

  return 0;
}

int pd_remove_page(char *v_addr) {
  u32 *pte;

  if (get_p_addr(v_addr)) {
    pte = (u32 *) (0xFFC00000 | (((u32) v_addr & 0xFFFFF000) >> 10));
    *pte = (*pte & (~PAGE_PRESENT));
    asm("invlpg %0"::"m"(v_addr));
  }

  return 0;
}

char *get_p_addr(char *v_addr) {
  u32 *pde;
  u32 *pte;

  pde = (u32 *) (0xFFFFF000 | (((u32) v_addr & 0xFFC00000) >> 20));
  if ((*pde & PAGE_PRESENT)) {
    pte = (u32 *) (0xFFC00000 | (((u32) v_addr & 0xFFFFF000) >> 10));

    if ((*pte & PAGE_PRESENT)) {
      return (char *) ((*pte & 0xFFFFF000) + (VADDR_PG_OFFSET((u32) v_addr)));
    }
  }

  return 0;
}
