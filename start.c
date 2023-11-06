// include what you want to execute
#include "printf.h"
#include "serial.h"

__attribute__((naked, section(".init"))) void _start(void) {
  // execute your os here
  printf("Type a character and shrineOS will translate it!\n\r");
  printf("Terminate the OS with 'q'!\n\r");

  for (;;) {
    char c = serial_read();
    printf("You gave the char: %c\n\r", c);

    if (c == 'q') {
      printf("And now, I'll terminate myself...\n\r");
      break;
    }

    printf("shrineOS says: %c\n\r", c + 1);
  }

  // then use an endless loop (for now)
  // for(;;);
}
