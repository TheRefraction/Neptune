#include "lib/string.h"
#include "paging.h"

#define __KERNEL_PROCESS__
#include "process.h"

void load_task(u32* code_phys_addr, u32* fn, u32 code_size) {
  memcpy((char*) code_phys_addr, (char*) fn, code_size);

  // Update Page Bitmap
  u32 page_base = (u32) PAGE(code_phys_addr);
  u32 pages = code_size / PAGE_SIZE;

  if (code_size % PAGE_SIZE) {
    pages++; 
  }

  for (u32 i = 0; i < pages; i++) {
    set_page_frame_used(page_base + i);
  }

  // Virtual addressing space creation
  u32* pd = pd_create(code_phys_addr, code_size);

  // Registers initialization for current task
  p_list[n_proc].pid = n_proc;
  p_list[n_proc].regs.ss = 0x33;
  p_list[n_proc].regs.esp = 0x40001000;
  p_list[n_proc].regs.cs = 0x23;
  p_list[n_proc].regs.eip = 0x40000000;
  p_list[n_proc].regs.ds = 0x2B;
  p_list[n_proc].regs.cr3 = (u32) pd;

  n_proc++;
}
