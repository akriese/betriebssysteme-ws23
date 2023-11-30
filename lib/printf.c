#include <stdarg.h> // for va_list etc., can be imported, as not OS specific

#include <dbgu.h>

#define HEX_POSITIONS 8 // 1 hex char represents 4 bits; assuming 32bit integers

/*
 * Some function that actually prints a character or sends it via the serial
 * interface
 */
void _print(char c) { dbgu_getc(c); }

void _printChar(const char c);
void _printString(const char *s);
void _printHex(const int u);
void _printAddress(const void *p);

/*
 * Our printf implementation
 *
 */
__attribute__((format(printf, 1, 2))) void printf(const char *fmt, ...) {
  va_list arguments;

  va_start(arguments, fmt);

  while (*fmt) {
    if (*fmt != '%') {
      _print(*fmt);
      fmt++;
      continue;
    }

    char f = *(++fmt);

    // in case there is a trailing '%'
    if (!f) {
      _print('%');
      break;
    }

    switch (f) {
    case 'c':
      _printChar((char)va_arg(arguments, int));
      break;
    case 's':
      _printString(va_arg(arguments, char *));
      break;
    case 'x':
      _printHex(va_arg(arguments, int));
      break;
    case 'p':
      _printAddress(va_arg(arguments, void *));
      break;
    default:
      _printString("Invalid formatter!");
    }

    fmt++;
  }

  va_end(arguments);
}

void _printChar(const char c) { _print(c); }

void _printString(const char *s) {
  while (*s) {
    _print(*s++);
  }
}

char _singleHex(int u) {
  if (u < 10)
    return '0' + u;
  return 'a' + u - 10;
}

/**
 * Prints a given integer in hex representation.
 * Cuts off leading zeros.
 */
void _printHex(const int u) {
  char hex[HEX_POSITIONS + 1]; // including trailing null terminator

  hex[HEX_POSITIONS] = '\0';

  int firstIdx = HEX_POSITIONS - 1;

  int i;
  for (i = 0; i < HEX_POSITIONS; ++i) {
    char res = _singleHex(u >> (i * 4) & 0xF);
    int idx = HEX_POSITIONS - i - 1;
    hex[idx] = res;

    if (res != '0')
      firstIdx = idx;
  }

  // using the string formatting of our own printf to prepend the "0x"
  printf("0x%s", hex + firstIdx);
}

void _printAddress(const void *p) { _printHex((int)p); }
