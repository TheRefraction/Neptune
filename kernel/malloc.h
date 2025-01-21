#define MALLOC_MINSIZE    16

#include "types.h"

struct malloc_header {
  unsigned long size:31;
  unsigned long used:1;
} __attribute__((packed));

void *sbrk(int);
void *malloc(unsigned long);
void free(void *);

