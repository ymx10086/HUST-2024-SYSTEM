#include "klib.h"
#include <stdarg.h>

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE 1
#endif

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAXN 100000

int printf(const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);

  char out[MAXN]; 
  int length = vsprintf(out, fmt, vl);
  for(int i = 0; i < length; i++) {
    _putc(out[i]);
  }
  va_end(vl);

  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int format = FALSE;
  int longarg = FALSE;
  size_t pos = 0;

  for (; *fmt; fmt++) {
    if (format) {
      switch (*fmt) {
        case 'l':
          longarg = TRUE;
          break;
        case 'p':
          longarg = TRUE;
          if (++pos < MAXN) out[pos - 1] = '0';
          if (++pos < MAXN) out[pos - 1] = 'x';
        case 'x': {
          long num = longarg ? va_arg(ap, long) : va_arg(ap, int);
          for (int i = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1; i >= 0; i--) {
            int d = (num >> (4 * i)) & 0xF;
            if (++pos < MAXN) out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
          }
          longarg = FALSE;
          format = FALSE;
          break;
        }
        case 'd': {
          long num = longarg ? va_arg(ap, long) : va_arg(ap, int);
          if (num < 0) {
            num = -num;
            if (++pos < MAXN) out[pos - 1] = '-';
          }
          long digits = 1;
          for (long nn = num; nn /= 10; digits++)
            ;
          for (int i = digits - 1; i >= 0; i--) {
            if (pos + i + 1 < MAXN) out[pos + i] = '0' + (num % 10);
            num /= 10;
          }
          pos += digits;
          longarg = FALSE;
          format = FALSE;
          break;
        }
        case 's': {
          const char* s2 = va_arg(ap, const char*);
          while (*s2) {
            if (++pos < MAXN) out[pos - 1] = *s2;
            s2++;
          }
          longarg = FALSE;
          format = FALSE;
          break;
        }
        case 'c': {
          if (++pos < MAXN) out[pos - 1] = (char)va_arg(ap, int);
          longarg = FALSE;
          format = FALSE;
          break;
        }
        default:
          break;
      }
    } else if (*fmt == '%')
      format = TRUE;
    else if (++pos < MAXN)
      out[pos - 1] = *fmt;
  }
  if (pos < MAXN)
    out[pos] = '\0';
  else if (MAXN)
    out[MAXN - 1] = '\0';
  return pos;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);

  int length = vsprintf(out,fmt,vl);

  va_end(vl);
  return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
