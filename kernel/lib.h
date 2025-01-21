#ifndef __KERNEL_LIB__
#define __KERNEL_LIB__

#include "types.h"

void *memcpy(char *, char *, u32);
u32 strcpy(char *, char *);
u8 strcmp(char *, char *);
u32 strlen(char *);
void itoa(char *, u32, u8);
void printf(char *, ...);

#endif