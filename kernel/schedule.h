#ifndef __KERNEL_SCHEDULER__
#define __KERNEL_SCHEDULER__

void schedule(void);
void switch_to_task(int, int);

#endif