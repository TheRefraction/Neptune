#include "types.h"
#include "gdt.h"
#include "process.h"

void switch_to_task(int n, int mode) {
  u32 k_esp, eflags;
  u16 k_ss, ss, cs;

  current = &p_list[n];

  // Load TSS 
  default_tss.ss0 = current->kstack.ss0;
  default_tss.esp0 = current->kstack.esp0;

  ss = current->regs.ss;
  cs = current->regs.cs;
  eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF;

  if (mode == KERNEL_MODE) {
    k_ss = current->regs.ss;
    k_esp = current->regs.esp;
  } else {
    k_ss = current->kstack.ss0;
    k_esp = current->kstack.esp0;
  }

  asm("mov %0, %%ss; \
      mov %1, %%esp; \
      cmp %[KMODE], %[mode]; \
      je next; \
      push %2; \
      push %3; \
      next: \
      push %4; \
      push %5; \
      push %6; \
      push %7; \
      ljmp $0x08, $do_switch" :: \
      "m"(k_ss), "m"(k_esp), "m"(ss), "m"(current->regs.esp), "m"(eflags), "m"(cs), \
      "m"(current->regs.eip), "m"(current), [KMODE] "i"(KERNEL_MODE), [mode] "g"(mode));
}

void schedule(void) {
  u32* stack_ptr;
  struct process *p;

  asm("mov (%%ebp), %%eax; \
      mov %%eax, %0" : "=m"(stack_ptr) :);

  // No process loaded, but one is ready
  if (current == 0 && n_proc) {
    switch_to_task(0, USER_MODE);
  } else if (n_proc <= 1) { // No scheduling for one task
    return;
  } else if (n_proc > 1) {
    // Save current registers
    current->regs.eflags = stack_ptr[16];
    current->regs.cs  = stack_ptr[15];
    current->regs.eip = stack_ptr[14];
    current->regs.eax = stack_ptr[13];
    current->regs.ecx = stack_ptr[12];
    current->regs.edx = stack_ptr[11];
    current->regs.ebx = stack_ptr[10];
    current->regs.ebp = stack_ptr[8];
    current->regs.esi = stack_ptr[7];
    current->regs.edi = stack_ptr[6];
    current->regs.ds = stack_ptr[5];
    current->regs.es = stack_ptr[4];
    current->regs.fs = stack_ptr[3];
    current->regs.gs = stack_ptr[2];

    // Interrupt
    if (current->regs.cs == 0x08) {
      current->regs.esp = 12 + stack_ptr[9];
      current->regs.ss = default_tss.ss0;
    } else {
      current->regs.esp = stack_ptr[17];
      current->regs.ss = stack_ptr[18]; 
    }

    current->kstack.ss0 = default_tss.ss0;
    current->kstack.esp0 = default_tss.esp0;

    // Get new process pid 
    if (n_proc > 1 + current->pid) {
      p = &p_list[1 + current->pid];
    } else {
      p = &p_list[0];
    }

    // Switch task
    if (p->regs.cs == 0x08) {
      switch_to_task(p->pid, KERNEL_MODE);
    } else {
      switch_to_task(p->pid, USER_MODE);
    }
  }
}
