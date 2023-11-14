// include what you want to execute
#include "printf.h"
#include "serial.h"

int main() {
  char *intro = "Type a character and shrineOS will translate it!";
  printf("%s (string stored at %x)\n\r", intro, intro);
  printf("Terminate the OS with 'q'!\n\r");

  for (;;) {
    char c = serial_read();
    printf("You gave the char: %c (%x)\n\r", c, c);

    if (c == 'q') {
      printf("And now, I'll terminate myself...\n\r");
      break;
    }

    printf("shrineOS says: %c\n\r", c + 1);
  }

  return 0;
}
