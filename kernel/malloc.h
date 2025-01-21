#include "types.h"

#define MALLOC_MINSIZE    16

struct malloc_header {
  u32 size:31;
  u32 used:1;
} __attribute__((packed));

void *sbrk(u32);
void *malloc(u32);
void free(void *);

