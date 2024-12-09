#include "libk.h"

void *memcpy(char *dst, char *src, int n) {
    char *p = dst;
    while (n--) {
        *dst++ = *src++;
    }
    return p;
}
