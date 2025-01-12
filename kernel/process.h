#include "types.h"

struct process {
  u32 pid;
  struct {
    u32 eax, ecx, edx, ebx;
    u32 esp, ebp, esi, edi;
    u32 eip, eflags;
    u32 cs:16, ss:16, ds:16, es:16, fs:16, gs:16;
    u32 cr3;
  } regs __attribute__((packed));
} __attribute__ ((packed));

#ifdef __KERNEL_PROCESS__
  struct process p_list[32];
  struct process* current = 0;
  u32 n_proc = 0;
#else 
  extern struct process p_list[];
  extern struct process* current;
  extern u32 n_proc;
#endif

void load_task(u32*, u32*, u32);
