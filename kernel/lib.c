#include <stdarg.h>
#include "tty.h"

void *memcpy(char *dst, char *src, u32 n) {
	char *p = dst;
	while (n--)
		*dst++ = *src++;
	return p;
}

u32 strcpy(char *dst, char *src) {
	u32 i = 0;
	while ((dst[i] = src[i++]));

	return i;
}

u8 strcmp(char *dst, char *src) {
	u32 i = 0;

	while ((dst[i] == src[i])) {
		if (src[i++] == 0)
			return 0;
	}

	return 1;
}

u32 strlen(char *s){
	u32 i = 0;
	while (*s++) {
		i++;
	}

	return i;
}

void itoa(char *buffer, u32 n, u8 base) {
	u32 tmp = n;
	u32 i = 0;

	do {
		tmp = n % base;
		buffer[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
	} while (n /= base);

	buffer[i--] = 0;

	for (u32 j = 0; j < i; j++, i--) {
		tmp = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = tmp;
	}
}

