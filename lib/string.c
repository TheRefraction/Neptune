#include "string.h"

void* memset(void* ptr, int value, size_t n) {
	if (!ptr) {
		return 0;
	}

	unsigned char* cptr = (unsigned char*) ptr;

	while (n--) {
		*cptr++ = value;
	}

	return ptr;
}

void* memcpy(void* dest, const void* src, size_t n) {
	if (!dest && !src) {
		return 0;
	}

	unsigned char* cdest = (unsigned char*) dest;
	const unsigned char* csrc = (const unsigned char*) src;

	while (n--) {
		*cdest++ = *csrc++;
	}

	return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
	if (!dest && !src) {
		return 0;
	}

	unsigned char* cdest = (unsigned char*) dest;
	const unsigned char* csrc = (const unsigned char*) src;
	
	if (dest > src) {
		while (n--) {
			*(cdest + n) = *(csrc + n);
		}
	} else {
		while (n--) {
			*cdest++ = *csrc++;
		}
	}

	return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t n) {
	if (!ptr1 && !ptr2) {
		return 0;
	}

	const unsigned char* a = (const unsigned char*) ptr1;
	const unsigned char* b = (const unsigned char*) ptr2;
	
	while (n--) {
		if (*a++ != *b++) {
			return a[-1] < b[-1] ? -1 : 1;
		}
	}

	return 0;
}
