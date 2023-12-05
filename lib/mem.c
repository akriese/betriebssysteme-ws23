#include <mem.h>

void *memcpy(void *src, void *dest, unsigned int size) {
  const char *s = src;
  char *d = dest;

  while (size--) {
    *d++ = *s++;
  }

  return dest;
}
