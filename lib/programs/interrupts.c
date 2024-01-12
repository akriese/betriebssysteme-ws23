#include <print.h>
#include <shrine_os_lib.h>
#include <system.h>

void timer_interrupt_printer() { print("!\n\r"); }

/**
 * @brief Reacts to system interrupts.
 *
 * Prints '!' (plus newline) on system timer IRQ and the typed letter 40x when
 * the DBGU receives a character.
 *
 * ATTENTION: This program was first introduced in `u03`. Since then, the OS has
 * undergone some changes which broke it (syscalls, etc.).
 * Thus, the program has been altered to accomodate for those changes.
 * Check out the git tag `u03` to see the actual implementation.
 */
int interrupt_program() {
  print("start of the initialization\n\r");

  // initialize the IRQ routines for receiving system timer fires
  sys_call_register_irq_callback(SYSTEM_TIMER_HANDLER, timer_interrupt_printer);

  print("setup done!\n\r");

  while (1) {
    char c = sys_call_read_char();

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
