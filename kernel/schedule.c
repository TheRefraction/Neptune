#include "types.h"
#include "gdt.h"
#include "process.h"

void schedule(void) {
  u32* stack_ptr;
  u32 esp0, eflags;
  u16 ss, cs;

  asm("mov (%%ebp), %%eax \n \
      mov %%eax, %0" : "=m"(stack_ptr) :);

  // No process loaded, but one is ready
  if (current == 0 && n_proc) {
    current = &p_list[0];
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

    current->regs.esp = stack_ptr[17];
    current->regs.ss = stack_ptr[18];

    default_tss.esp0 = (u32) (stack_ptr + 19);

    // Get new process 
    if (n_proc > 1 + current->pid) {
      current = &p_list[1 + current->pid];
    } else {
      current = &p_list[0];
    }
  }

  // Push registers onto the stack, restore registers of new task and switch
  ss = current->regs.ss;
  cs = current->regs.cs;
  eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF;
  esp0 = default_tss.esp0;
  
  asm("	mov %0, %%esp \n \
    push %1 \n \
    push %2 \n \
    push %3 \n \
    push %4 \n \
    push %5 \n \
    push %6 \n \
    ljmp $0x08, $do_switch"
    :: "m" (esp0), "m" (ss), "m" (current->regs.esp), "m" (eflags), "m" (cs), "m" (current->regs.eip), "m" (current));
}
