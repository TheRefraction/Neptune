#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
void* memmove(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t);

#endif
