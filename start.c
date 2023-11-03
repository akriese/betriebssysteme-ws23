// include what you want to execute

#include "serial.h"

__attribute__((naked, section(".init")))
void _start(void) {
  // execute your os here
  for (;;) {
    char c = read();
    write_string("\n\rYou gave the char:");
    write(c);
    write_string("\n\rI say:");
    write(c+1);
    if (c == 'q') {
      write_string("\n\rAnd now I'll terminate myself...");
      break;
    }
  }

  // then use an endless loop (for now)
  // for(;;);
}
