#include "types.h"
#include "io.h"
#include "lib.h"
#include "mem.h"
#include "malloc.h"

#define __KERNEL_PROCESS__
#include "process.h"

int load_task(char *fn, u32 code_size) {
  struct page_directory *pd;
  struct page_list *pglist;
  struct page *kstack;

  char *v_addr, *p_addr, *ustack;

  u32 pid, i;

  // FIXME: Reuse free slots 
  pid = 1;
  while (p_list[pid].state != 0 && pid++ < MAX_PROCESS);

  if (p_list[pid].state != 0) {
    printf("ERROR: load_task(): Not enough slots for new process!\n");
    return 0;
  }

  pd = pd_create();

  asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"(pd->base->p_addr));

  pglist = (struct page_list *) malloc(sizeof(struct page_list));
  pglist->page = 0;
  pglist->next = 0;
  pglist->prev = 0;

  i = 0;
  while (i < code_size) {
    p_addr = get_page_frame();
    v_addr = (char *) (USER_OFFSET + i);
    
    pd_add_page(v_addr, p_addr, PAGE_USER, pd);

    pglist->page = (struct page *) malloc(sizeof(struct page));
    pglist->page->p_addr = p_addr;
    pglist->page->v_addr = v_addr;

    pglist->next = (struct page_list *) malloc(sizeof(struct page_list));
    pglist->next->page = 0;
    pglist->next->next = 0;
    pglist->next->prev = pglist;

    pglist = pglist->next;

    i += PAGE_SIZE;
  }

  memcpy((char *) USER_OFFSET, fn, code_size);

  ustack = get_page_frame();
  pd_add_page((char *) USER_STACK, ustack, PAGE_USER, pd);

  kstack = get_page_from_heap();

  n_proc++;

  p_list[pid].pid = pid;

  p_list[pid].regs.ss     = 0x33;
  p_list[pid].regs.esp    = USER_STACK + PAGE_SIZE - 16;
  p_list[pid].regs.eflags = 0x0;
  p_list[pid].regs.cs     = 0x23; // Offset in GDT + ring 3 
  p_list[pid].regs.eip    = 0x40000000;
  p_list[pid].regs.ds     = 0x2B; // Offset in GDT + ring 3 
  p_list[pid].regs.es     = 0x2B;
  p_list[pid].regs.fs     = 0x2B;
  p_list[pid].regs.gs     = 0x2B;

  p_list[pid].regs.cr3    = (u32) pd->base->p_addr;

  p_list[pid].kstack.ss0  = 0x18; // Offset in GDT
  p_list[pid].kstack.esp0 = (u32) kstack->v_addr + PAGE_SIZE - 16;

  p_list[pid].regs.eax    = 0;
  p_list[pid].regs.ecx    = 0;
  p_list[pid].regs.edx    = 0;
  p_list[pid].regs.ebx    = 0;

  p_list[pid].regs.ebp    = 0;
  p_list[pid].regs.esi    = 0;
  p_list[pid].regs.edi    = 0;

  p_list[pid].pd          = pd;
  p_list[pid].pglist      = pglist;

  p_list[pid].state       = 1; // Loaded

  asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"(current->regs.cr3));

  return pid;
}
