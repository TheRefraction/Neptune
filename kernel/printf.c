#include <stdarg.h>

#include "types.h"
#include "tty.h"
#include "lib.h"

void printf(char *s, ...) {
  va_list ap;

  char buffer[16];
  u32 size, buflen, uival;
  int i, j;
  u8 c, neg;

  va_start(ap, s);

  while((c = *s++)) {
    size = 0;
    neg = 0;

    if (c == '\0') {
      break;
    } else if (c == '%') {
      c = *s++;
      if (c >= '0' && c <= '9') {
        size = c - '0';
        c = *s++;
      }

      switch(c) {
        case 'd': {
          int ival = va_arg(ap, int);
          if (ival < 0) {
            uival = 0 - ival;
            neg++;
          } else {
            uival = ival;
          }

          itoa(buffer, uival, 10);
          buflen = strlen(buffer);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buffer[i] = (j >= 0) ? buffer[j] : '0';
            }
          }

          if (neg) {
            printf("-%s", buffer);
          } else {
            printf(buffer);
          }
          break;
        }
        case 'u': {
          uival = va_arg(ap, int);
          
          itoa(buffer, uival, 10);
          buflen = strlen(buffer);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buffer[i] = (j >= 0) ? buffer[j] : '0';
            } 
          }

          printf(buffer);
          break;
        }
        case 'x': {
          uival = va_arg(ap, int);
          
          itoa(buffer, uival, 16);
          buflen = strlen(buffer);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buffer[i] = (j >= 0) ? buffer[j] : '0';
            } 
          }

          printf("0x%s", buffer);
          break;
        }
        case 'p': {
          uival = va_arg(ap, int);
          size = 8;
          
          itoa(buffer, uival, 16);
          buflen = strlen(buffer);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buffer[i] = (j >= 0) ? buffer[j] : '0';
            } 
          }

          printf("0x%s", buffer);
          break;
        }
        case 's': {
          printf((char*) va_arg(ap, int));
          break;
        }
      }
    } else {
      terminal_putchar(c);
    }
  }

  va_end(ap);

  return;
}
