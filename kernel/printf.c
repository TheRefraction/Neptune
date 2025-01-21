#include <stdarg.h>

#include "tty.h"
#include "lib.h"

void printf(char *s, ...) {
  va_list ap;

  char buf[16];
  unsigned int size, buflen;
  unsigned char neg;
  int i, j;

  unsigned char c;
  int ival;
  unsigned int uival;

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
          ival = va_arg(ap, int);
          if (ival < 0) {
            uival = 0 - ival;
            neg++;
          } else {
            uival = ival;
          }

          itoa(buf, uival, 10);
          buflen = strlen(buf);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buf[i] = (j >= 0) ? buf[j] : '0';
            }
          }

          if (neg) {
            printf("-%s", buf);
          } else {
            printf(buf);
          }
          break;
        }
        case 'u': {
          uival = va_arg(ap, int);
          
          itoa(buf, uival, 10);
          buflen = strlen(buf);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buf[i] = (j >= 0) ? buf[j] : '0';
            } 
          }

          printf(buf);
          break;
        }
        case 'x': {
          uival = va_arg(ap, int);
          
          itoa(buf, uival, 16);
          buflen = strlen(buf);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buf[i] = (j >= 0) ? buf[j] : '0';
            } 
          }

          printf("0x%s", buf);
          break;
        }
        case 'p': {
          uival = va_arg(ap, int);
          size = 8;
          
          itoa(buf, uival, 16);
          buflen = strlen(buf);

          if (buflen < size) {
            for (i = size, j = buflen; i >= 0; i--, j--) {
              buf[i] = (j >= 0) ? buf[j] : '0';
            } 
          }

          printf("0x%s", buf);
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
