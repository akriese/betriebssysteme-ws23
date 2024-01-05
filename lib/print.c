#include <stdarg.h> // for va_list etc., can be imported, as not OS specific

#include <dbgu.h>
#include <sys_call.h>

/*
 * Some function that actually prints a character or sends it via the serial
 * interface
 */
void _print(char c) { sys_call_put_char(c); }

void _printDecimal(const int d);
void _printChar(const char c);
void _printString(const char *s);
void _printHex(const int u, const int full);
void _printAddress(const void *p);
void _printBinary(const unsigned int d);
void _printInBase(const unsigned int d, const char bitsPerChar,
                  const char *alphabet, const char *prefix, const int full);

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
    case 'b':
      _printBinary(va_arg(arguments, int));
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

  int is_negative = d < 0;

  if (is_negative) {
    x = -x;
  }

  do {
    digits[idx--] = lookup[x % 10];
    x /= 10;
  } while (x != 0);

  if (is_negative) {
    digits[idx--] = '-';
  }

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
  _printInBase(u, 4, "0123456789abcdef", "0x", full);
}

void _printBinary(const unsigned int d) { _printInBase(d, 1, "01", "0b", 1); }

/**
 * This only works for bases that are a power of 2 currently.
 */
void _printInBase(const unsigned int d, const char bitsPerChar,
                  const char *alphabet, const char *prefix, const int full) {
  unsigned int positions = (int)32 / bitsPerChar;

  // mask to get only the last bits of the number for the character
  const int mask = (1 << bitsPerChar) - 1;

  char output[positions + 1]; // including trailing null terminator

  output[positions] = '\0';

  unsigned int firstIdx = positions - 1;

  unsigned int i;
  for (i = 0; i < positions; ++i) {
    char res = alphabet[d >> (i * bitsPerChar) & mask];
    int idx = positions - i - 1;
    output[idx] = res;

    if (res != '0' || full)
      firstIdx = idx;
  }

  // using the string formatting of our own printf to prepend the "0x"
  print("%s%s", prefix, output + firstIdx);
}

void _printAddress(const void *p) { _printHex((int)p, 1); }
