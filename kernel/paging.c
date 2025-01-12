#include "types.h"

#define __KERNEL_PAGING__
#include "paging.h"

char* get_page_frame(void) {
  int page = -1;

  for (int byte = 0; byte < PAGE_MAX / 8; byte++) {
    if (mem_bitmap[byte] != 0xFF) {
      for (u8 bit = 0; bit < 8; bit++) {
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
  for (i = 0; i < RAM_MAXPAGE / 8; i++) {
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
	  mov %%eax, %%cr0" :: "i"(pd0), "i"(PAGING_FLAG));
}

u32* pd_create_task(void) {
  int i;

  u32* pd = (u32*) get_page_frame();
  for (i = 0; i < 1024; i++) {
    pd[i] = 0x0;
  }

  u32* pt = (u32*) get_page_frame();
  for (i = 0; i < 1024; i++) {
    pt[i] = 0x0;
  }

  pd[0] = (pd0[0] | PAGE_PRESENT | PAGE_RW);
  
  pd[USER_OFFSET >> 22] = (u32) (pt | PAGE_PRESENT | PAGE_RW | PAGE_USER);

  pt[0] = 0x100000;
  pt[0] |= (PAGE_PRESENT + PAGE_RW + PAGE_USER);

  return pd;
}
