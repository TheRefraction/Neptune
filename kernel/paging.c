#include "types.h"

#define __KERNEL_PAGING__
#include "paging.h"

char* get_page_frame(void) {
  int page = -1;

  int byte, bit;

  for (byte = 0; byte < PAGE_MAX / 8; byte++) {
    if (mem_bitmap[byte] != 0xFF) {
      for (bit = 0; bit < 8; bit++) {
        if (!(mem_bitmap[byte] & (1 << bit))) {
          page = 8 * byte + bit;
          // Set the found page to reserved/used
          set_page_frame_used(page);
          return (char*) (page * PAGE_SIZE);
        }
      }
    }
  }

  // No available page
  return (char*) -1;
}

// Fonction d'initialisation de la pagination (Adresse virtuelle = adresse physique) sur 4 Mio
void init_paging(void) {
  u32 page_addr = 0x0;
  int i;

  // Bitmap initialization
  for (i = 0; i < PAGE_MAX / 8; i++) {
    mem_bitmap[i] = 0;
  }

  // Reserved pages for kernel
  for (i = PAGE(0x0); i < PAGE(0x20000); i++) {
    set_page_frame_used(i);
  }

  // Reserved pages for hardware (screen, keyboard, ...)
  for (i = PAGE(0xA0000); i < PAGE(0x100000); i++) {
    set_page_frame_used(i);
  }

  // Initialize Page directory and first Page table
  pd0 = (u32*) get_page_frame();
  pt0 = (u32*) get_page_frame();

  for (i = 0; i < 1024; i++) {
    pd0[i] = 0x0;
  }

  // Add first table to directory
  pd0[0] = (u32) pt0;
  pd0[0] |= PAGE_PRESENT;
  pd0[0] |= PAGE_RW;

  // Initialize first page table
  for (i = 0; i < 1024; i++) {
    pt0[i] = page_addr;
    pt0[i] |= PAGE_PRESENT;
    pt0[i] |= PAGE_RW;

    page_addr += PAGE_SIZE;
  }


    /*
     * Passage de l'adresse du répertoire de pages au registre CR3
     * Mise à 1 du bit de pagination (n°31) dans le registre CR0
     */
  asm("mov %0, %%eax \n \
    mov %%eax, %%cr3 \n \
	  mov %%cr0, %%eax \n \
	  or %1, %%eax \n \
	  mov %%eax, %%cr0" :: "m"(pd0), "i"(PAGING_FLAG));
}

u32* pd_create(u32* code_phys_addr, u32 code_size) {
  u32 i, j;

  // Initialize page directory 
  u32* pd = (u32*) get_page_frame();
  for (i = 0; i < 1024; i++) {
    pd[i] = 0;
  }

  // Kernel space (to allow syscalls)
  pd[0] = pd0[0];
  pd[0] |= PAGE_PRESENT;
  pd[0] |= PAGE_RW;

  // User space 
  u32 pages = code_size / PAGE_SIZE;
  if (code_size % PAGE_SIZE) {
    pages++;
  }

  for (i = 0; pages; i++) {
    u32* pt = (u32*) get_page_frame();
    
    pd[(USER_OFFSET + i * PAGE_SIZE * 1024) >> 22] = (u32) pt;
    pd[(USER_OFFSET + i * PAGE_SIZE * 1024) >> 22] |= 7;

    for (j = 0; j < 1024 && pages; j++, pages--) {
      pt[j] = (u32) (code_phys_addr + i * PAGE_SIZE * 1024 + j * PAGE_SIZE);
      pt[j] |= 7;
    }
  }

  return pd;
}
