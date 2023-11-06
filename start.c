// include what you want to execute

#include "serial.h"

__attribute__((naked, section(".init"))) void _start(void) {
  // execute your os here
  write_string("Type a character and shrineOS will translate it!\n\r");
  write_string("Terminate the OS with 'q'!\n\r");

  for (;;) {
    char c = read();
    write_string("You gave the char: ");
    write(c);

    if (c == 'q') {
      write_string("\n\rAnd now, I'll terminate myself...");
      break;
    }

    write_string("\n\rshrineOS says: ");
    write(c + 1);
    write_string("\n\r");
  }

  // then use an endless loop (for now)
  // for(;;);
}
