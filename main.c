// include what you want to execute
#include "printf.h"
#include "serial.h"

extern int _perform_reset(void);
extern int _perform_int(void);

int main() {
  char *intro = "Type a character and shrineOS will translate it!";
  printf("%s (string stored at %x)\n\r", intro, intro);
  printf("Terminate the OS with 'q'!\n\r");

  for (;;) {
    char c = serial_read();
    printf("You gave the char: %c (%x)\n\r", c, c);

    int break_loop = 0;

    switch (c) {
    case 'Q':
      printf("'Performing' a system reset...\n\r\n\r");
      _perform_reset();
      break;
    case 'I':
      _perform_int();
      break;
    case 'U':
      // a GCC internal function that executes an undefined instruction
      __builtin_trap();
      break;
    case 'P':
      // prefetch abort
      break;
    case 'D':
      // data abort
      break;
    case 'S':
      // software interrupt
      break;
    case 'q':
      printf("And now, I'll terminate myself...\n\r");
      break_loop = 1;
      break;
    default:
      printf("shrineOS says: %c\n\r", c + 1);
    }

    if (break_loop)
      break;
  }

  return 0;
}

int _print_mem(int address, int content) {
  printf("%x: %x\n\r", address, content);
  return 0;
}

int _print_exception(int type) {
  char *text;

  switch (type) {
  case 0:
    text = "reset";
    break;
  case 1:
    text = "undefined instruction";
    break;
  case 2:
    text = "software interrupt";
    break;
  case 3:
    text = "prefetch abort";
    break;
  case 4:
    text = "data abort";
    break;
  case 5:
    text = "irq";
    break;
  case 6:
    text = "fiq";
    break;
  default:
    text = "UNKNOWN";
  }

  printf("Exception triggered: %s\n\r", text);
  return 0;
}
