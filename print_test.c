/*
 * A simple program to test the custom printf function.
 *
 * Compile and run with: gcc -Wall -Wextra -O2 print_test.c && ./a.out
 */

#include "printf.h"
#include <stdio.h>

void _print(char c) { printf("%c", c); }

int main() {
  char *someText = "Hello World!";

  // to compare our print and stdlib's printf
  print("%s, %c, %x, %p\n", someText, 'x', 4120129, someText);
  printf("%s, %c, %x, %p\n", someText, 'x', 4120129, someText);
}
