#include "io.h"
#include "lib.h"
#include "mem.h"
#include "malloc.h"

void *sbrk(u32 n) {
  struct malloc_header *chunk;
  char *p_addr;

  if ((krnl_heap + (n * PAGE_SIZE)) > (char*) KRNL_HEAP_LIM) {
    printf("ERROR: sbrk(): no virtual memory left for kernel heap!\n");
    return (char*) -1;
  }

  chunk = (struct malloc_header *) krnl_heap;

  for (u32 i = 0; i < n; i++) {
    p_addr = get_page_frame();
    if (p_addr < 0) {
      printf("ERROR: sbrk(): no free page frame available!\n");
      return (char*) -1;
    }

    pd0_add_page(krnl_heap, p_addr, 0);

    krnl_heap += PAGE_SIZE;
  }

  chunk->size = PAGE_SIZE * n;
  chunk->used = 0;

  return chunk;
}

void *malloc(u32 size) {
  struct malloc_header *chunk, *other;

  u32 realsize = sizeof(struct malloc_header) + size;
  if (realsize < MALLOC_MINSIZE) {
    realsize = MALLOC_MINSIZE;
  }

  chunk = (struct malloc_header*) KRNL_HEAP;
  while (chunk->used || chunk->size < realsize) {
    if (chunk->size == 0) {
      printf("PANIC: malloc(): corrupted chunk on %x with null size (heap %x)!\n", chunk, krnl_heap);
      hlt;
    }

    chunk = (struct malloc_header *) ((char *) chunk + chunk->size);

    if (chunk == (struct malloc_header *) krnl_heap) {
      if (sbrk((realsize / PAGE_SIZE) + 1) < 0) {
        printf("PANIC: malloc(): no memory left for kernel!\n");
        hlt;
      }
    } else if (chunk > (struct malloc_header *) krnl_heap) {
      printf("PANIC: malloc(): chunk on %x while heap limit on %x!\n", chunk, krnl_heap);
      hlt;
    }
  }

  if (chunk->size - realsize < MALLOC_MINSIZE) {
    chunk->used = 1;
  } else {
    other = (struct malloc_header *) ((char *) chunk + realsize);
    other->size = chunk->size - realsize;
    other->used = 0;

    chunk->size = realsize;
    chunk->used = 1;
  }

  return (char *) chunk + sizeof(struct malloc_header);
}

void free(void *v_addr) {
  struct malloc_header *chunk, *other;

  chunk = (struct malloc_header *) (v_addr - sizeof(struct malloc_header));
  chunk->used = 0;

  while ((other = (struct malloc_header *) ((char *) chunk + chunk->size)) && other < (struct malloc_header *) krnl_heap && other->used == 0) {
    chunk->size += other->size;
  }
}
