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

    if (c == 'Q') {
      printf("'Performing' a system reset...\n\r\n\r");
      _perform_reset();
    }

    if (c == 'I') {
      _perform_int();
    }

    if (c == 'q') {
      printf("And now, I'll terminate myself...\n\r");
      break;
    }

    printf("shrineOS says: %c\n\r", c + 1);
  }

  return 0;
}

int test_print(void) {
  printf("Performed an interrupt!\n\r");
  return 0;
}
