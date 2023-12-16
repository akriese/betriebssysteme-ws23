#include <dbgu.h>
#include <print.h>
#include <system.h>

extern void cpsr_enable_interrupts();

int interrupt_program() {
  print("start of the initialization\n\r");

  // enable interrupts in the cpu
  cpsr_enable_interrupts();

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // activate the system timer and enable its interrupts
  st_activate_pits(1000); // interrupt about every second

  // install handler for system interrupts (dbgu and system timer, currently)
  install_interrupt_handlers();

  print("setup done!\n\r");

  while (1) {
    char c = dbgu_getc();

    volatile int i; // volatile so it is not optimized away
    for (i = 0; i < 40; i++) {
      print("%c", c);

      // dummy count j up to waste some time
      volatile int j;
      // modify the bound to vary the speed of printing the character
      const int wait_bound = 10000000;
      for (j = 0; j < wait_bound; j++) {
      }
    }
  }

  return 0;
}
