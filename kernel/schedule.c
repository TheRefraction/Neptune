#include "types.h"
#include "gdt.h"
#include "process.h"
#include "lib.h"
#include "mem.h"

void switch_to_task(int n, int mode) {
  u32 kesp, eflags;
  u16 kss, ss, cs;

  current = &p_list[n];

  // Load TSS
  default_tss.ss0 = current->kstack.ss0;
  default_tss.esp0 = current->kstack.esp0;

  ss = current->regs.ss;
  cs = current->regs.cs;
  eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF;

  if (mode == USER_MODE) {
		kss = current->kstack.ss0;
		kesp = current->kstack.esp0;
	} else {			/* KERNELMODE */
		kss = current->regs.ss;
		kesp = current->regs.esp;
	}

  asm("mov %0, %%ss; \
      mov %1, %%esp; \
      push %%eax; \
      mov %[KMODE], %%eax; \
      cmp %%eax, %[mode]; \
      pop %%eax; \
      je next; \
      push %2; \
      push %3; \
      next: \
      push %4; \
      push %5; \
      push %6; \
      push %7; \
      ljmp $0x08, $do_switch" 
      :: \
      "m"(kss), "m"(kesp), "m"(ss), "m"(current->regs.esp), "m"(eflags), "m"(cs), \
      "m"(current->regs.eip), "m"(current), [KMODE] "i"(KERNEL_MODE), [mode] "g"(mode));
}

void schedule(void) {
  struct process *p;
  u32 *stack_ptr;
  int i, newpid;

  asm("mov (%%ebp), %%eax; mov %%eax, %0":"=m"(stack_ptr):);

  if (!n_proc) {
    return;
  } else if (n_proc == 1 && current->pid != 0) {
    return;
  } else {
    // Save current registers
    current->regs.eflags    = stack_ptr[16];
    current->regs.cs        = stack_ptr[15];
    current->regs.eip       = stack_ptr[14];
    current->regs.eax       = stack_ptr[13];
    current->regs.ecx       = stack_ptr[12];
    current->regs.edx       = stack_ptr[11];
    current->regs.ebx       = stack_ptr[10];

    current->regs.ebp       = stack_ptr[8];
    current->regs.esi       = stack_ptr[7];
    current->regs.edi       = stack_ptr[6];

    current->regs.ds        = stack_ptr[5];
    current->regs.es        = stack_ptr[4];
    current->regs.fs        = stack_ptr[3];
    current->regs.gs        = stack_ptr[2];

    if (current->regs.cs != 0x08) {
      current->regs.esp     = stack_ptr[17];
      current->regs.ss      = stack_ptr[18];
    } else {
      current->regs.esp     = stack_ptr[9] + 12;
      current->regs.ss      = default_tss.ss0;
    }

    current->kstack.ss0     = default_tss.ss0;
    current->kstack.esp0    = default_tss.esp0;
  }

  // Get new process pid
  newpid = 0;
  for (i = current->pid + 1; i < MAX_PROCESS && newpid == 0; i++) {
    if (p_list[i].state == 1) {
      newpid = i;
    }
  }

  if (!newpid) {
    for (i = 1; i < current->pid && newpid == 0; i++) {
      if (p_list[i].state == 1) {
        newpid = i;
      }
    }
  }

  p = &p_list[newpid];

  // Switch task
  if (p->regs.cs != 0x08) {
    switch_to_task(p->pid, USER_MODE);
  } else {
    switch_to_task(p->pid, KERNEL_MODE);
  }
}
