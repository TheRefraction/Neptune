#include "lib/string.h"
#include "paging.h"
#include "tty.h"

#define __KERNEL_PROCESS__
#include "process.h"

void load_task(u32* code_phys_addr, u32* fn, u32 code_size) {
  u32 page_base, pages, kstack_base;

  memcpy((char*) code_phys_addr, (char*) fn, code_size);

  // Update Page Bitmap
  page_base = (u32) PAGE(code_phys_addr);
  pages = code_size / PAGE_SIZE;

  if (code_size % PAGE_SIZE) {
    pages++; 
  }

  int i;
  for (i = 0; i < pages; i++) {
    set_page_frame_used(page_base + i);
  }

  // Virtual addressing space creation
  u32* pd = pd_create(code_phys_addr, code_size);

  kstack_base = (u32) get_page_frame();
  if (kstack_base > 0x400000) {
    terminal_write("Error! Not enough memory for kernel stack!\n");
    return;
  }

  // Registers initialization for current task
  p_list[n_proc].pid = n_proc;
  p_list[n_proc].regs.ss = 0x33;
  p_list[n_proc].regs.esp = 0x40001000;
  p_list[n_proc].regs.eflags = 0x0;
  p_list[n_proc].regs.cs = 0x23;
  p_list[n_proc].regs.eip = 0x40000000;
  p_list[n_proc].regs.ds = 0x2B;
  p_list[n_proc].regs.es = 0x2B;
  p_list[n_proc].regs.fs = 0x2B;
  p_list[n_proc].regs.gs = 0x2B;
  p_list[n_proc].regs.cr3 = (u32) pd;

  p_list[n_proc].kstack.ss0 = 0x18; // Offset in GDT
  p_list[n_proc].kstack.esp0 = kstack_base + PAGE_SIZE;

  p_list[n_proc].regs.eax = 0x0;
  p_list[n_proc].regs.ecx = 0x0;
  p_list[n_proc].regs.edx = 0x0;
  p_list[n_proc].regs.ebx = 0x0;

  p_list[n_proc].regs.ebp = 0x0;
  p_list[n_proc].regs.esi = 0x0;
  p_list[n_proc].regs.edi = 0x0;

  n_proc++;
}
