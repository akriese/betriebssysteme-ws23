#include <mem.h>

void *memcpy(void *src, void *dest, unsigned int size) {
  const char *s = src;
  char *d = dest;

  while (size--) {
    *d++ = *s++;
  }

  return dest;
}

void memset(void *dest, char content, unsigned int size) {
  char *d = dest;

  while (size--) {
    *d++ = content;
  }
}
