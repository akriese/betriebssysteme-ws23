#include <stdarg.h> // for va_list etc., can be imported, as not OS specific

#include <dbgu.h>

#define HEX_POSITIONS 8 // 1 hex char represents 4 bits; assuming 32bit integers

/*
 * Some function that actually prints a character or sends it via the serial
 * interface
 */
void _print(char c) { dbgu_putc(c); }

void _printDecimal(const int d);
void _printChar(const char c);
void _printString(const char *s);
void _printHex(const int u, const int full);
void _printAddress(const void *p);

/*
 * Our printf implementation
 *
 */
__attribute__((format(printf, 1, 2))) void print(const char *fmt, ...) {
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
    case 'd':
      _printDecimal(va_arg(arguments, int));
      break;
    case 'c':
      _printChar((char)va_arg(arguments, int));
      break;
    case 's':
      _printString(va_arg(arguments, char *));
      break;
    case 'x':
      _printHex(va_arg(arguments, int), 0);
      break;
    case 'X':
      _printHex(va_arg(arguments, int), 1);
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

void _printDecimal(const int d) {
  int x = d;

  const char *lookup = "0123456789";
  char digits[16];

  // filling up the digits string from the right (small positions first)
  digits[15] = '\0';
  int idx = 14;

  do {
    digits[idx--] = lookup[x % 10];
    x /= 10;
  } while (x > 0);

  _printString(digits + idx + 1);
}

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
void _printHex(const int u, const int full) {
  char hex[HEX_POSITIONS + 1]; // including trailing null terminator

  hex[HEX_POSITIONS] = '\0';

  int firstIdx = HEX_POSITIONS - 1;

  int i;
  for (i = 0; i < HEX_POSITIONS; ++i) {
    char res = _singleHex(u >> (i * 4) & 0xF);
    int idx = HEX_POSITIONS - i - 1;
    hex[idx] = res;

    if (res != '0' || full)
      firstIdx = idx;
  }

  // using the string formatting of our own printf to prepend the "0x"
  print("0x%s", hex + firstIdx);
}

void _printAddress(const void *p) { _printHex((int)p, 1); }
