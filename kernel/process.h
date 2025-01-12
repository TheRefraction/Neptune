#include "types.h"

#define MAX_PROCESS 16

struct REGS {
  u32 eax, ecx, edx, ebx;
  u32 esp, ebp, esi, edi;
  u32 eip, eflags;
  u32 cs:16, ss:16, ds:16, es:16, fs:16, gs:16;
  u32 cr3;
} __attribute__((packed));

struct process {
  u32 pid;
  struct REGS regs;
} __attribute__ ((packed));

#ifdef __KERNEL_PROCESS__
  struct process p_list[MAX_PROCESS];
  struct process* current = 0;
  int n_proc = 0;
#else 
  extern struct process p_list[];
  extern struct process* current;
  extern int n_proc;
#endif

void load_task(u32*, u32*, u32);
